#pragma once

constexpr char S2C_P_PLAYER_INFO = 1;
constexpr char S2C_P_MOVE = 2;
constexpr char S2C_P_ATTACK = 3;
constexpr char S2C_P_GAME_STATE = 4;
constexpr char C2S_P_LOGIN = 5;
constexpr char C2S_P_MOVE = 6;
constexpr char C2S_P_ATTACK = 7;
constexpr char S2C_P_PLAYER_LEAVE = 11;
constexpr char S2C_P_GAME_START = 12;
constexpr char MAX_ID_LEN = 20;

struct Vector3 {
    float x;
    float y;
    float z;
};

#pragma pack(push, 1)

struct cs_packet_login {
    unsigned char size;
    char type;
    char username[MAX_ID_LEN];
};

struct sc_packet_login_ok {
    unsigned char size;
    char type;
    long long playerId;
    Vector3 position;
    int health;
    float walkSpeed;
    float runSpeed;
    int faintCount;
    bool isFainted;
};

struct cs_packet_move {
    unsigned char size;
    char type;
    Vector3 direction;
    float yaw;
};

struct sc_packet_move {
    unsigned char size;
    char type;
    long long playerId;
    Vector3 position;
    float yaw;
};

struct cs_packet_attack {
    unsigned char size;
    char type;
    Vector3 attackDirection;
};

struct sc_packet_attack {
    unsigned char size;
    char type;
    long long playerId;
    long long zombieId;
    Vector3 impactPoint;
};

struct sc_packet_player_leave {
    unsigned char size;
    char type;
    long long playerId;
};

struct sc_packet_game_start {
    unsigned char size;
    char type;
};

#pragma pack(pop)
