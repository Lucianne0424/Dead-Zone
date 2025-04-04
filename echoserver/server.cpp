#include "server.h"
#include "GameRoom.h"         // ���ӷ� ���
#include "GameManager.h"      // ���̺� ���� ���� ���
#include "db_authentication.h"// ����� ����/ȸ������ �Լ� ���� (authenticateUser, registerUser)
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

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 9000
#define NUM_WORKER_THREADS 4

// ���� ����
HANDLE g_hIOCP = NULL;                           // IOCP �ڵ�
std::mutex g_lobbyMutex;                         // ���� ť ��ȣ�� ���ؽ�
std::queue<PER_SOCKET_CONTEXT*> g_lobbyQueue;      // ����(�κ�) ť
std::vector<GameRoom*> g_gameRooms;              // ������ ���ӷ���� ����

// ���� �Լ�: �ش� �÷��̾ ���� GameRoom�� ã�� ��ȯ (������ nullptr)
GameRoom* FindGameRoomForPlayer(PER_SOCKET_CONTEXT* player) {
    for (auto room : g_gameRooms) {
        for (auto p : room->players) {
            if (p == player) {
                return room;
            }
        }
    }
    return nullptr;
}

// �Լ� ����
void PostRecv(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData);
void ProcessClientMessage(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData, int bytesTransferred);
void MatchmakingCheck();
void WorkerThread(HANDLE hIOCP);

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
            printf("WSASend ����: %d\n", err);
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
            printf("WSARecv ����: %d\n", err);
        }
    }
}

void WorkerThread(HANDLE hIOCP) {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    LPOVERLAPPED pOverlapped;

    while (true) {
        BOOL result = GetQueuedCompletionStatus(hIOCP, &bytesTransferred, &completionKey, &pOverlapped, INFINITE);
        if (!result) {
            printf("GetQueuedCompletionStatus ����: %d\n", GetLastError());
            continue;
        }
        if (pOverlapped == NULL)
            break; // ���� ��ȣ

        PER_IO_DATA* pIoData = (PER_IO_DATA*)pOverlapped;
        PER_SOCKET_CONTEXT* pContext = (PER_SOCKET_CONTEXT*)completionKey;

        if (pIoData->operationType == IO_READ) {
            if (bytesTransferred == 0) {
                printf("Ŭ���̾�Ʈ ����: socket %d\n", pContext->socket);
                closesocket(pContext->socket);
                delete pContext;
                delete pIoData;
                continue;
            }
            ProcessClientMessage(pContext, pIoData, bytesTransferred);
            ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));
            PostRecv(pContext, pIoData);
        }
        // IO_WRITE �Ϸ� �� Ư���� ó���� ����
    }
}

void ProcessClientMessage(PER_SOCKET_CONTEXT* pContext, PER_IO_DATA* pIoData, int bytesTransferred) {
    pIoData->buffer[bytesTransferred] = '\0';
    std::string msg(pIoData->buffer);
    printf("socket %d �κ��� ���� �޽���: %s\n", pContext->socket, msg.c_str());

    std::istringstream iss(msg);
    std::string command;
    iss >> command;

    if (pContext->state == STATE_GAME) {
        if (command == "MOVE") {
            float dx, dy;
            iss >> dx >> dy;
            pContext->moveX = dx;
            pContext->moveY = dy;
            std::string reply = "MOVE_DIRECTION_SET: (" + std::to_string(dx) + ", " + std::to_string(dy) + ")\n";
            PER_IO_DATA* sendData = new PER_IO_DATA;
            PostSend(pContext, reply, sendData);
        }
        else if (command == "ATTACK") {
            std::string reply = "ATTACK_COMMAND_RECEIVED\n";
            PER_IO_DATA* sendData = new PER_IO_DATA;
            PostSend(pContext, reply, sendData);
        }
        else if (command == "STATUS") {
            std::string reply = "STATUS: HP=" + std::to_string(pContext->health) +
                ", Pos=(" + std::to_string(pContext->posX) + ", " + std::to_string(pContext->posY) + ")\n";
            PER_IO_DATA* sendData = new PER_IO_DATA;
            PostSend(pContext, reply, sendData);
        }
        else if (command == "REVIVE") {
            if (pContext->isFainted && pContext->faintCount == 1) {
                pContext->isFainted = false;
                pContext->health = 100;
                std::string reply = "PLAYER_REVIVED\n";
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
            else {
                std::string reply = "REVIVE_FAILED\n";
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
        }
        else if (command == "STATUS_ALL") {
            GameRoom* room = FindGameRoomForPlayer(pContext);
            if (room) {
                std::ostringstream oss;
                oss << "ALLIED STATUS:\n";
                for (auto ally : room->players) {
                    oss << ally->username << " - HP: " << ally->health
                        << ", Pos: (" << ally->posX << ", " << ally->posY << ")\n";
                }
                std::string reply = oss.str();
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
            else {
                std::string reply = "STATUS_ALL_FAILED: ���ӷ� ���� ����\n";
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
        }
        else {
            std::string reply = "GAME: " + msg;
            PER_IO_DATA* sendData = new PER_IO_DATA;
            PostSend(pContext, reply, sendData);
        }
    }
    else if (pContext->state == STATE_LOGIN) {
        if (command == "LOGIN") {
            std::string username, password;
            iss >> username >> password;
            if (!username.empty() && !password.empty()) {
                // MySQL ���� �κ��� �ּ� ó���ϰ� �׻� �����ϵ��� ��.
                // ���� ���� �ÿ��� �ؽ� ó���� �ʿ��մϴ�.
                bool authResult = true; // �ӽ�: �׻� ����

                if (authResult) {
                    pContext->username = username;
                    pContext->state = STATE_LOBBY;
                    pContext->health = 100;
                    pContext->posX = 0.0f;
                    pContext->posY = 0.0f;
                    pContext->walkSpeed = 3.0f;
                    pContext->runSpeed = 5.0f;
                    pContext->faintCount = 0;
                    pContext->isFainted = false;
                    pContext->moveX = 0.0f;
                    pContext->moveY = 0.0f;

                    std::string reply = "LOGIN_OK\n";
                    PER_IO_DATA* sendData = new PER_IO_DATA;
                    PostSend(pContext, reply, sendData);

                    {
                        std::lock_guard<std::mutex> lock(g_lobbyMutex);
                        g_lobbyQueue.push(pContext);
                    }
                    MatchmakingCheck();
                }
                else {
                    std::string reply = "LOGIN_FAIL: ���� ����\n";
                    PER_IO_DATA* sendData = new PER_IO_DATA;
                    PostSend(pContext, reply, sendData);
                }
            }
            else {
                std::string reply = "LOGIN_FAIL: �ùٸ� ����: LOGIN <username> <password>\n";
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
        }
        else if (command == "SIGNUP") {
            std::string username, password;
            iss >> username >> password;
            if (!username.empty() && !password.empty()) {
                // MySQL ȸ������ �κ��� �ּ� ó���ϰ� �׻� �����ϵ��� ��.
                bool signupResult = true; // �ӽ�: �׻� ����

                if (signupResult) {
                    std::string reply = "SIGNUP_OK\n";
                    PER_IO_DATA* sendData = new PER_IO_DATA;
                    PostSend(pContext, reply, sendData);
                }
                else {
                    std::string reply = "SIGNUP_FAIL: ȸ������ ����\n";
                    PER_IO_DATA* sendData = new PER_IO_DATA;
                    PostSend(pContext, reply, sendData);
                }
            }
            else {
                std::string reply = "SIGNUP_FAIL: �ùٸ� ����: SIGNUP <username> <password>\n";
                PER_IO_DATA* sendData = new PER_IO_DATA;
                PostSend(pContext, reply, sendData);
            }
        }
        else {
            std::string reply = "�α��� ���¿��� LOGIN/SIGNUP�� �����մϴ�.\n";
            PER_IO_DATA* sendData = new PER_IO_DATA;
            PostSend(pContext, reply, sendData);
        }
    }
    else if (pContext->state == STATE_LOBBY) {
        std::string reply = "LOBBY: " + msg;
        PER_IO_DATA* sendData = new PER_IO_DATA;
        PostSend(pContext, reply, sendData);
    }
}

void MatchmakingCheck() {
    std::lock_guard<std::mutex> lock(g_lobbyMutex);
    while (g_lobbyQueue.size() >= 3) {
        PER_SOCKET_CONTEXT* player1 = g_lobbyQueue.front(); g_lobbyQueue.pop();
        PER_SOCKET_CONTEXT* player2 = g_lobbyQueue.front(); g_lobbyQueue.pop();
        PER_SOCKET_CONTEXT* player3 = g_lobbyQueue.front(); g_lobbyQueue.pop();

        player1->state = STATE_GAME;
        player2->state = STATE_GAME;
        player3->state = STATE_GAME;

        std::vector<PER_SOCKET_CONTEXT*> roomPlayers = { player1, player2, player3 };
        GameRoom* newRoom = new GameRoom(roomPlayers);
        g_gameRooms.push_back(newRoom);

        {
            PER_IO_DATA* sendData1 = new PER_IO_DATA;
            PER_IO_DATA* sendData2 = new PER_IO_DATA;
            PER_IO_DATA* sendData3 = new PER_IO_DATA;
            std::string startMsg = "GAME_START\n";
            PostSend(player1, startMsg, sendData1);
            PostSend(player2, startMsg, sendData2);
            PostSend(player3, startMsg, sendData3);
        }
        printf("���ӷ� ����: %s, %s, %s\n",
            player1->username.c_str(),
            player2->username.c_str(),
            player3->username.c_str());
    }
}

int main() {
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        printf("WSAStartup ����: %d\n", wsaResult);
        return 1;
    }

    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        printf("���� ���� ���� ����: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("bind ����: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen ����: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("���� ����: ��Ʈ %d\n", DEFAULT_PORT);

    g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (g_hIOCP == NULL) {
        printf("IOCP ���� ����: %d\n", GetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::vector<std::thread> workerThreads;
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        workerThreads.push_back(std::thread(WorkerThread, g_hIOCP));
    }

    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept ����: %d\n", WSAGetLastError());
            continue;
        }
        printf("Ŭ���̾�Ʈ ����: socket %d\n", clientSocket);

        PER_SOCKET_CONTEXT* pContext = new PER_SOCKET_CONTEXT;
        pContext->socket = clientSocket;
        pContext->state = STATE_LOGIN;
        pContext->username = "";
        pContext->health = 100;
        pContext->posX = 0.0f;
        pContext->posY = 0.0f;
        pContext->walkSpeed = 3.0f;
        pContext->runSpeed = 5.0f;
        pContext->faintCount = 0;
        pContext->isFainted = false;
        pContext->moveX = 0.0f;
        pContext->moveY = 0.0f;

        CreateIoCompletionPort((HANDLE)clientSocket, g_hIOCP, (ULONG_PTR)pContext, 0);

        PER_IO_DATA* pIoData = new PER_IO_DATA;
        ZeroMemory(&pIoData->overlapped, sizeof(OVERLAPPED));
        pIoData->operationType = IO_READ;
        pIoData->wsabuf.buf = pIoData->buffer;
        pIoData->wsabuf.len = MAX_BUFFER;
        PostRecv(pContext, pIoData);
    }

    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        PostQueuedCompletionStatus(g_hIOCP, 0, 0, NULL);
    }
    for (auto& th : workerThreads) {
        th.join();
    }

    CloseHandle(g_hIOCP);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
