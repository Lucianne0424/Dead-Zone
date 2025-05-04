#include "server.h"
#include "GameRoom.h"
#include "GameManager.h"
//#include "db_authentication.h"
#include "protocol.h"

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT      9000
#define NUM_WORKER_THREADS 4
#define NUM_POST_ACCEPTS   10

SOCKET g_listenSocket = INVALID_SOCKET;
HANDLE  g_hIOCP = NULL;
std::mutex g_lobbyMutex;
std::queue<PER_SOCKET_CONTEXT*> g_lobbyQueue;
std::mutex g_playersMutex;
std::vector<PER_SOCKET_CONTEXT*> g_connectedPlayers;

std::atomic<bool> g_running{ true };

LPFN_ACCEPTEX lpfnAcceptEx = NULL;

GameRoom* FindGameRoomForPlayer(PER_SOCKET_CONTEXT* player) {
    for (auto* room : activeRooms) {
        for (auto* p : room->players) {
            if (p == player)
                return room;
        }
    }
    return nullptr;
}

void PostAccept(SOCKET listenSocket);
void PostRecv(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData);
void PostSendPacket(PER_SOCKET_CONTEXT* pContext, const void* packet, size_t packetSize);
void ProcessClientMessage(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData, int bytesTransferred);
void MatchmakingCheck();
void WorkerThread(HANDLE hIOCP);

void PostSendPacket(PER_SOCKET_CONTEXT* pContext, const void* packet, size_t packetSize) {
    PER_IO_DATA* pIoData = new PER_IO_DATA;
    memcpy(pIoData->buffer, packet, packetSize);
    pIoData->wsabuf.buf = pIoData->buffer;
    pIoData->wsabuf.len = static_cast<ULONG>(packetSize);
    pIoData->operationType = IO_WRITE;
    ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));

    DWORD bytesSent = 0;
    int result = WSASend(pContext->socket, &pIoData->wsabuf, 1, &bytesSent, 0, &pIoData->overlapped, NULL);
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            printf("WSASend error: %d\n", err);
            delete pIoData;
        }
    }
}

void PostSend(PER_SOCKET_CONTEXT* pContext, const std::string& msg, PER_IO_DATA* pIoData) {
    strcpy_s(pIoData->buffer, MAX_BUFFER, msg.c_str());
    pIoData->wsabuf.buf = pIoData->buffer;
    pIoData->wsabuf.len = (ULONG)msg.length();
    pIoData->operationType = IO_WRITE;
    ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));

    DWORD bytesSent = 0;
    int result = WSASend(pContext->socket, &pIoData->wsabuf, 1, &bytesSent, 0, &pIoData->overlapped, NULL);
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            printf("WSASend 에러: %d\n", err);
            delete pIoData;
        }
    }
}

void PostAccept(SOCKET listenSocket) {
    PER_IO_DATA* pAcceptIoData = new PER_IO_DATA;
    pAcceptIoData->operationType = IO_ACCEPT;
    pAcceptIoData->acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pAcceptIoData->acceptSocket == INVALID_SOCKET) {
        printf("새 accept 소켓 생성 실패: %d\n", WSAGetLastError());
        delete pAcceptIoData;
        return;
    }
    ZeroMemory(&pAcceptIoData->overlapped, sizeof(OVERLAPPED));

    DWORD bytesReceived = 0;
    if (lpfnAcceptEx(listenSocket, pAcceptIoData->acceptSocket, pAcceptIoData->buffer, 0,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
        &bytesReceived, &pAcceptIoData->overlapped) == FALSE) {
        int err = WSAGetLastError();
        if (err != ERROR_IO_PENDING) {
            printf("AcceptEx 호출 실패: %d\n", err);
            closesocket(pAcceptIoData->acceptSocket);
            delete pAcceptIoData;
        }
    }
}

void PostRecv(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData) {
    pIoData->wsabuf.buf = pIoData->buffer;
    pIoData->wsabuf.len = MAX_BUFFER;
    pIoData->operationType = IO_READ;
    ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));

    DWORD flags = 0, bytesRecv = 0;
    int result = WSARecv(pContext->socket, &pIoData->wsabuf, 1, &bytesRecv, &flags, &pIoData->overlapped, NULL);
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            printf("WSARecv 에러: %d\n", err);
            delete pIoData;
        }
    }
}

void WorkerThread(HANDLE) {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    LPOVERLAPPED pOverlapped;

    while (true) {
        BOOL result = GetQueuedCompletionStatus(
            g_hIOCP, &bytesTransferred, &completionKey, &pOverlapped, INFINITE);
        DWORD lastErr = result ? 0 : GetLastError();
            if (!result && pOverlapped != nullptr &&
                (lastErr == ERROR_NETNAME_DELETED || lastErr == ERROR_OPERATION_ABORTED)) {
                bytesTransferred = 0;
            
        }
         else if (!result) {
            printf("GetQueuedCompletionStatus 에러: %d\n", lastErr);
            continue;
            
        }
        if (pOverlapped == NULL)  
            break;

        auto* pIoData = (PER_IO_DATA*)pOverlapped;
        auto* pContext = (PER_SOCKET_CONTEXT*)completionKey;

        if (pIoData->operationType == IO_ACCEPT) {
            
            SOCKET acceptedSocket = pIoData->acceptSocket;
            if (setsockopt(acceptedSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                (char*)&g_listenSocket, sizeof(g_listenSocket)) == SOCKET_ERROR) {
                printf("SO_UPDATE_ACCEPT_CONTEXT 실패: %d\n", WSAGetLastError());
                closesocket(acceptedSocket);
                delete pIoData;
                continue;
            }

            PER_SOCKET_CONTEXT* newContext = new PER_SOCKET_CONTEXT;
            newContext->socket = acceptedSocket;
            newContext->state = STATE_LOGIN;
            newContext->username.clear();
            newContext->health = 100;
            newContext->posX = newContext->posY = newContext->posZ = 0.0f;
            newContext->walkSpeed = 3.0f;
            newContext->runSpeed = 5.0f;
            newContext->faintCount = 0;
            newContext->isFainted = false;
            newContext->moveX = newContext->moveY = newContext->moveZ = 0.0f;
            newContext->isJumping = false;
            newContext->verticalVelocity = 0.0f;

            CreateIoCompletionPort((HANDLE)acceptedSocket, g_hIOCP, (ULONG_PTR)newContext, 0);
            PER_IO_DATA* pRecvIoData = new PER_IO_DATA;
            pRecvIoData->operationType = IO_READ;
            pRecvIoData->wsabuf.buf = pRecvIoData->buffer;
            pRecvIoData->wsabuf.len = MAX_BUFFER;
            ZeroMemory(&pRecvIoData->overlapped, sizeof(OVERLAPPED));
            PostRecv(newContext, pRecvIoData);

            {
                std::lock_guard<std::mutex> lock(g_playersMutex);
                g_connectedPlayers.push_back(newContext);
            }
            printf("새 연결 수락: socket %d\n", acceptedSocket);

            PostAccept(g_listenSocket);
            delete pIoData;
        }
        else if (pIoData->operationType == IO_READ) {
            if (bytesTransferred == 0) {
                printf("클라이언트 종료: socket %d\n", pContext->socket);

                if (auto* room = FindGameRoomForPlayer(pContext)) {
                    auto& vec = room->players;
                    vec.erase(std::remove(vec.begin(), vec.end(), pContext),
                        vec.end());
                    
                    if (vec.empty()) {
                        delete room;  
                    }
                }

                sc_packet_player_leave leavePacket{};
                leavePacket.size = sizeof(sc_packet_player_leave);
                leavePacket.type = S2C_P_PLAYER_LEAVE;
                leavePacket.playerId = pContext->socket;
                if (auto* room = FindGameRoomForPlayer(pContext)) {
                    for (auto* peer : room->players)
                        PostSendPacket(peer, &leavePacket, leavePacket.size);
                }

                {
                    std::lock_guard<std::mutex> lock(g_playersMutex);
                    g_connectedPlayers.erase(
                        std::remove(g_connectedPlayers.begin(),
                            g_connectedPlayers.end(),
                            pContext),
                        g_connectedPlayers.end());
                }
                closesocket(pContext->socket);
                delete pContext;
                delete pIoData;
                continue;
            }
            ProcessClientMessage(pContext, pIoData, bytesTransferred);

            ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));
            PostRecv(pContext, pIoData);
        }
    }
}

void ProcessClientMessage(PER_SOCKET_CONTEXT* pContext,
    PER_IO_DATA* pIoData,
    int                 bytesTransferred)
{
    if (bytesTransferred < 2) return;
    unsigned char packetSize = pIoData->buffer[0];
    char          packetType = pIoData->buffer[1];
    printf("socket %d → Size=%d, Type=%d\n", pContext->socket, packetSize, packetType);

    switch (packetType) {
    case C2S_P_LOGIN: {
        pContext->username = "Player_" + std::to_string(pContext->socket);
        pContext->state = STATE_LOBBY;
        pContext->health = 100;
        pContext->posX = 0.0f;
        pContext->posY = 500.0f;
        pContext->posZ = 0.0f;
        pContext->walkSpeed = 3.0f;
        pContext->runSpeed = 5.0f;
        pContext->faintCount = 0;
        pContext->isFainted = false;
        pContext->moveX = 0.0f;
        pContext->moveY = 0.0f;
        pContext->moveZ = 0.0f;
        pContext->isJumping = false;
        pContext->verticalVelocity = 0.0f;

        sc_packet_login_ok loginOk;
        loginOk.size = sizeof(sc_packet_login_ok);
        loginOk.type = S2C_P_PLAYER_INFO;
        loginOk.playerId = pContext->socket;
        loginOk.position = { pContext->posX, pContext->posY, pContext->posZ };
        loginOk.health = pContext->health;
        loginOk.walkSpeed = pContext->walkSpeed;
        loginOk.runSpeed = pContext->runSpeed;
        loginOk.faintCount = pContext->faintCount;
        loginOk.isFainted = pContext->isFainted;

        PostSendPacket(pContext, &loginOk, loginOk.size);

        {
            std::lock_guard<std::mutex> lock(g_lobbyMutex);
            g_lobbyQueue.push(pContext);
        }

        MatchmakingCheck();
        break;
    }

    case C2S_P_MOVE: {
        if (bytesTransferred < sizeof(cs_packet_move)) break;
        auto* pkt = reinterpret_cast<cs_packet_move*>(pIoData->buffer);
        Vector3 dir = pkt->direction;

        // 서버 권한 위치 갱신
        pContext->moveX = dir.x;
        pContext->moveY = dir.y;
        pContext->moveZ = dir.z;
        pContext->posX += dir.x * pContext->walkSpeed * 0.05f;
        pContext->posY += dir.y * pContext->walkSpeed * 0.05f;
        pContext->posZ += dir.z * pContext->walkSpeed * 0.05f;

        sc_packet_move moveUpdate;
        moveUpdate.size = sizeof(sc_packet_move);
        moveUpdate.type = S2C_P_MOVE;
        moveUpdate.playerId = pContext->socket;
        moveUpdate.position = { pContext->posX, pContext->posY, pContext->posZ };
        moveUpdate.yaw = pkt->yaw;

        if (auto* room = FindGameRoomForPlayer(pContext)) {
            /*for (auto* peer : room->players) {
                PostSendPacket(peer, &moveUpdate, moveUpdate.size);
            }*/
        }
        break;
    }

    case C2S_P_ATTACK: {
        if (bytesTransferred < sizeof(cs_packet_attack)) break;
        auto* pkt = reinterpret_cast<cs_packet_attack*>(pIoData->buffer);

        sc_packet_attack attackEvent;
        attackEvent.size = sizeof(sc_packet_attack);
        attackEvent.type = S2C_P_ATTACK;
        attackEvent.playerId = pContext->socket;
        attackEvent.zombieId = rand() % 100 + 1;
        attackEvent.impactPoint = pkt->attackDirection;

        if (auto* room = FindGameRoomForPlayer(pContext)) {
            for (auto* peer : room->players) {
                PostSendPacket(peer, &attackEvent, attackEvent.size);
            }
        }
        break;
    }

    case C2S_P_JUMP: {
        if (bytesTransferred < sizeof(cs_packet_jump) || pContext->isJumping)
            break;

        auto* req = reinterpret_cast<cs_packet_jump*>(pIoData->buffer);
        float initialVelocity = req->initVelocity;

        pContext->verticalVelocity = initialVelocity;
        pContext->isJumping = true;

        sc_packet_jump jumpEvent;
        jumpEvent.size = sizeof(sc_packet_jump);
        jumpEvent.type = S2C_P_JUMP;
        jumpEvent.playerId = pContext->socket;
        jumpEvent.initVelocity = initialVelocity;

        if (auto* room = FindGameRoomForPlayer(pContext)) {
            /*for (auto* peer : room->players) {
                if (peer != pContext)
                    PostSendPacket(peer, &jumpEvent, jumpEvent.size);
            }*/
        }
        break;
    }

    default: {
        printf("정의되지 않은 패킷 타입: %d\n", packetType);
        break;
    }
    }
}

    void MatchmakingCheck() {
    std::lock_guard<std::mutex> lock(g_lobbyMutex);
    while (g_lobbyQueue.size() >= 3) {
        PER_SOCKET_CONTEXT* p1 = g_lobbyQueue.front(); g_lobbyQueue.pop();
        PER_SOCKET_CONTEXT* p2 = g_lobbyQueue.front(); g_lobbyQueue.pop();
        PER_SOCKET_CONTEXT* p3 = g_lobbyQueue.front(); g_lobbyQueue.pop();

        p1->state = p2->state = p3->state = STATE_GAME;
        std::vector<PER_SOCKET_CONTEXT*> players = { p1, p2, p3 };

        sc_packet_game_start gameStart{};
        gameStart.size = sizeof(sc_packet_game_start);
        gameStart.type = S2C_P_GAME_START;
        {
            std::lock_guard<std::mutex> lock2(g_playersMutex);
            for (auto* pl : players)
                PostSendPacket(pl, &gameStart, gameStart.size);
        }

        for (auto* pl : players) {
            sc_packet_login_ok info{};
            info.size = sizeof(sc_packet_login_ok);
            info.type = S2C_P_PLAYER_INFO;
            info.playerId = pl->socket;
            info.position = { pl->posX, pl->posY, pl->posZ };
            info.health = pl->health;
            info.walkSpeed = pl->walkSpeed;
            info.runSpeed = pl->runSpeed;
            info.faintCount = pl->faintCount;
            info.isFainted = pl->isFainted;

            for (auto* peer : players)
                PostSendPacket(peer, &info, info.size);
        }

        new GameRoom(players);

        printf("게임룸 생성: %s, %s, %s\n",
            p1->username.c_str(), p2->username.c_str(), p3->username.c_str());
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    g_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
        NULL, 0, WSA_FLAG_OVERLAPPED);
    if (g_listenSocket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN sa = {};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(DEFAULT_PORT);
    if (bind(g_listenSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR ||
        listen(g_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("bind/listen failed\n");
        closesocket(g_listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Listening on port %d\n", DEFAULT_PORT);

    g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    CreateIoCompletionPort((HANDLE)g_listenSocket, g_hIOCP, 0, 0);

    GUID guidAcceptEx = WSAID_ACCEPTEX;
    DWORD bytes = 0;
    WSAIoctl(g_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guidAcceptEx, sizeof(guidAcceptEx),
        &lpfnAcceptEx, sizeof(lpfnAcceptEx),
        &bytes, NULL, NULL);

    for (int i = 0; i < NUM_POST_ACCEPTS; ++i)
        PostAccept(g_listenSocket);

    // 1) 게임룸 업데이트용 스레드
    std::thread updateThread([]() {
        auto prev = std::chrono::steady_clock::now();
        while (g_running) {
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - prev).count();
            prev = now;
            for (auto* room : activeRooms)
                room->Update(dt);
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        });

    // 2) IOCP 워커 스레드
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_WORKER_THREADS; ++i)
        workers.emplace_back(WorkerThread, g_hIOCP);

    printf("Press Enter to exit.\n");
    getchar();

    // 3) 서버 종료 절차
    g_running = false;
    updateThread.join();

    for (int i = 0; i < NUM_WORKER_THREADS; ++i)
        PostQueuedCompletionStatus(g_hIOCP, 0, 0, nullptr);
    for (auto& th : workers)
        th.join();

    CloseHandle(g_hIOCP);
    closesocket(g_listenSocket);
    WSACleanup();
    return 0;
}
