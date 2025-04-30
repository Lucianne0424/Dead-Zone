#pragma once

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <string>
#include <vector>
#include "protocol.h"

#define MAX_BUFFER 8192

enum IO_OPERATION { IO_READ, IO_WRITE, IO_ACCEPT };
enum SESSION_STATE { STATE_LOGIN, STATE_LOBBY, STATE_GAME };

struct PER_IO_DATA {
    OVERLAPPED    overlapped;
    WSABUF        wsabuf;
    char          buffer[MAX_BUFFER];
    IO_OPERATION  operationType;
    SOCKET        acceptSocket;
};

struct PER_SOCKET_CONTEXT {
    SOCKET         socket;
    SESSION_STATE  state;
    std::string    username;
    int            health;
    float          posX, posY, posZ;
    float          walkSpeed, runSpeed;
    int            faintCount;
    bool           isFainted;
    float          moveX, moveY, moveZ;
};

// AcceptEx 함수 포인터
extern LPFN_ACCEPTEX lpfnAcceptEx;

// 방 목록
class GameRoom;
extern std::vector<GameRoom*> activeRooms;

// 네트워크 헬퍼
void PostSend(PER_SOCKET_CONTEXT* ctx, const std::string& msg, PER_IO_DATA* io);
void PostRecv(PER_SOCKET_CONTEXT* ctx, PER_IO_DATA* io);
void PostAccept(SOCKET listenSocket);

// 패킷 처리
void ProcessClientMessage(PER_SOCKET_CONTEXT* ctx, PER_IO_DATA* io, int bytesTransferred);
GameRoom* FindGameRoomForPlayer(PER_SOCKET_CONTEXT* player);