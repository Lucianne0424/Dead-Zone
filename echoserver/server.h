#pragma once

#include <winsock2.h>
#include <windows.h>
#include <string>

#define MAX_BUFFER 8192  // 임시 버퍼 크기 확장 ---- 추후에 바꿔야함

// I/O 작업 종류
enum IO_OPERATION {
    IO_READ,
    IO_WRITE,
    IO_ACCEPT
};

// 클라이언트 세션 상태
enum SESSION_STATE {
    STATE_LOGIN,  // 로그인 전
    STATE_LOBBY,  // 대기실(로비) 상태
    STATE_GAME    // 게임 상태 (플레이어 관련 명령 처리)
};

// I/O 작업 데이터 구조체
struct PER_IO_DATA {
    OVERLAPPED overlapped;      // 비동기 I/O용
    WSABUF wsabuf;              // 버퍼 정보
    char buffer[MAX_BUFFER];    // 실제 데이터 저장 버퍼
    IO_OPERATION operationType; // 현재 작업 종류
};

// 클라이언트 소켓 컨텍스트 (플레이어 정보 포함)
struct PER_SOCKET_CONTEXT {
    SOCKET socket;          // 클라이언트 소켓
    SESSION_STATE state;    // 현재 세션 상태
    std::string username;   // 로그인 시 입력한 사용자 이름

    // 플레이어 기본 상태
    int health;             // 체력 (초기 100)
    float posX, posY;       // 위치 (초기 (0,0))
    float walkSpeed;        // 걷는 속도 (예: 3 m/s)
    float runSpeed;         // 뛰는 속도 (예: 5 m/s)
    int faintCount;         // 기절 횟수 (최대 1회 허용)
    bool isFainted;         // 기절 상태 여부

    // 시간 기반 이동을 위한 현재 이동 방향
    // (예: moveX, moveY가 -1 ~ +1 사이의 값이면 그 방향으로 지속적 이동)
    float moveX, moveY;
};

// 비동기 전송 함수 선언 (server.cpp에서 구현)
void PostSend(PER_SOCKET_CONTEXT* pContext, const std::string& msg, PER_IO_DATA* pIoData);
