#pragma once

// 기존 프로토콜 상수들
constexpr char S2C_P_PLAYER_INFO = 1;  // 서버 → 클라이언트: 플레이어 정보 업데이트 (로그인, 상태 동기화)
constexpr char S2C_P_MOVE = 2;  // 서버 → 클라이언트: 이동 업데이트 (위치 broadcast)
constexpr char S2C_P_ATTACK = 3;  // 서버 → 클라이언트: 공격 이벤트 broadcast (좀비에 대한 결과)
constexpr char S2C_P_GAME_STATE = 4;  // 서버 → 클라이언트: 게임 상태 업데이트
constexpr char C2S_P_LOGIN = 5;  // 클라이언트 → 서버: 로그인 요청
constexpr char C2S_P_MOVE = 6;  // 클라이언트 → 서버: 이동 요청
constexpr char C2S_P_ATTACK = 7;  // 클라이언트 → 서버: 공격 요청

// 새로 추가: 플레이어 퇴장 알림 (브로드캐스트)
constexpr char S2C_P_PLAYER_LEAVE = 11;

// 새로 추가: 게임 시작 이벤트 (브로드캐스트)
constexpr char S2C_P_GAME_START = 12;

constexpr char MAX_ID_LEN = 20;

struct Vector3 {
    float x;
    float y;
    float z;
};

#pragma pack(push, 1)

// 클라이언트 → 서버 로그인 요청 패킷
struct cs_packet_login {
    unsigned char size;              // 전체 패킷 크기
    char type;                       // 프로토콜 타입: C2S_P_LOGIN
    char username[MAX_ID_LEN];       // 사용자명
};

// 서버 → 클라이언트 로그인 성공 및 플레이어 초기 정보 패킷
struct sc_packet_login_ok {
    unsigned char size;              // 전체 패킷 크기
    char type;                       // 프로토콜 타입: S2C_P_PLAYER_INFO
    long long playerId;              // 플레이어 고유 ID
    Vector3 position;                // 초기 위치 (x, y, z)
    int health;                      // 체력 
    float walkSpeed;                 // 걷는 속도 (3 m/s)
    float runSpeed;                  // 뛰는 속도 (5 m/s)
    int faintCount;                  // 기절 횟수 (최대 1회 허용)
    bool isFainted;                  // 기절 상태 여부
};

// 클라이언트 → 서버 이동 요청 패킷 (확장 버전: 회전 정보 포함)
struct cs_packet_move {
    unsigned char size;      // 전체 패킷 크기
    char type;               // 프로토콜 타입: C2S_P_MOVE
    Vector3 direction;       // 이동 방향
    float yaw;               // 좌우 회전 각도 
};

// 서버 → 클라이언트 이동 업데이트 패킷
struct sc_packet_move {
    unsigned char size;      // 전체 패킷 크기
    char type;               // 프로토콜 타입: S2C_P_MOVE
    long long playerId;      // 이동한 플레이어의 ID
    Vector3 position;        // 새로운 위치 (x, y, z)
    float yaw;               // 업데이트된 회전 각도
};

// 클라이언트 → 서버 공격 요청 패킷
struct cs_packet_attack {
    unsigned char size;              // 전체 패킷 크기
    char type;                       // 프로토콜 타입: C2S_P_ATTACK
    Vector3 attackDirection;         // 공격 방향 (총구 방향 혹은 근접 공격 방향)
    // 추가 정보 (무기 종류) 추가 
};

// 서버 → 클라이언트 공격 이벤트 패킷
struct sc_packet_attack {
    unsigned char size;              // 전체 패킷 크기
    char type;                       // 프로토콜 타입: S2C_P_ATTACK
    long long playerId;              // 공격을 수행한 플레이어의 ID
    long long zombieId;              // 공격 당한 좀비의 ID
    Vector3 impactPoint;             // 공격이 명중한 위치 (x, y, z)
};

// 서버 → 클라이언트 플레이어 퇴장 알림 패킷
struct sc_packet_player_leave {
    unsigned char size;  // 전체 패킷 크기
    char type;           // 프로토콜 타입: S2C_P_PLAYER_LEAVE
    long long playerId;  // 퇴장한 플레이어의 ID
};

// 서버 → 클라이언트 게임 시작 이벤트 패킷
struct sc_packet_game_start {
    unsigned char size;  // 전체 패킷 크기
    char type;           // 프로토콜 타입: S2C_P_GAME_START
};

#pragma pack(pop)
