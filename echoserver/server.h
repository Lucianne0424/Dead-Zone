// server.h (수정된 부분)
#pragma once
#include <winsock2.h>
#include <windows.h>
#include <string>

#define MAX_BUFFER 8192  // 임시 버퍼 크기 확장 ---- 추후에 변경

enum IO_OPERATION {
    IO_READ,
    IO_WRITE,
    IO_ACCEPT   // AcceptEx 전용
};

enum SESSION_STATE {
    STATE_LOGIN,  // 로그인 전
    STATE_LOBBY,  // 대기실(로비) 상태
    STATE_GAME    // 게임 상태 (플레이어 관련 명령 처리)
};

struct PER_IO_DATA {
    OVERLAPPED overlapped;      // 비동기 I/O용
    WSABUF wsabuf;              // 버퍼 정보
    char buffer[MAX_BUFFER];    // 실제 데이터 저장 버퍼
    IO_OPERATION operationType; // 현재 작업 종류
 
    SOCKET acceptSocket;
};

struct PER_SOCKET_CONTEXT {
    SOCKET socket;          // 클라이언트 소켓
    SESSION_STATE state;    // 현재 세션 상태
    std::string username;   // 로그인 시 입력한 사용자 이름

    // 플레이어 기본 상태
    int health;             // 체력 (초기 100)
    float posX, posY, posZ; // 위치 (초기 (0,0))
    float walkSpeed;        // 걷는 속도 (3 m/s)
    float runSpeed;         // 뛰는 속도 (5 m/s)
    int faintCount;         // 기절 횟수 (최대 1회 허용)
    bool isFainted;         // 기절 상태 여부

    // 이동 방향 (시간 기반 이동)
    float moveX, moveY, moveZ;
};

void PostSend(PER_SOCKET_CONTEXT* pContext, const std::string& msg, PER_IO_DATA* pIoData);
