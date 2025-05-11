#pragma once


constexpr char S2C_P_PLAYER_INFO = 1;
constexpr char S2C_P_MOVE = 2;
constexpr char S2C_P_ATTACK = 3;
constexpr char S2C_P_GAME_STATE = 4;
constexpr char C2S_P_LOGIN = 5;
constexpr char C2S_P_MOVE = 6;
constexpr char C2S_P_ATTACK = 7;
constexpr char C2S_P_JUMP = 8;
constexpr char S2C_P_JUMP = 9;
constexpr char S2C_P_LAND = 10;
constexpr char S2C_P_PLAYER_LEAVE = 11;
constexpr char S2C_P_GAME_START = 12;
constexpr char S2C_P_SNAPSHOT = 13;
constexpr char C2S_P_STATE = 14;
constexpr char S2C_P_STATE = 15;
constexpr char S2C_P_LOGIN_OK = 16;
constexpr char S2C_P_SPAWN_ZOMBIE = 17;
constexpr char S2C_P_ZOMBIE_MOVE = 18;
constexpr char S2C_P_ZOMBIE_STATE = 19;
constexpr char MAX_ID_LEN = 20;
constexpr char S2C_P_ZOMBIE_DIE = 21;

struct Vector3 {
    float x;
    float y;
    float z;
};

#pragma pack(push, 1)

struct cs_packet_login {
    unsigned char size;
    char          type;
    char          username[MAX_ID_LEN];
};

struct cs_packet_move {
    unsigned char size;
    char          type;
    Vector3       direction;
    Vector3       look;
};

struct cs_packet_attack {
    unsigned char size;
    char          type;
    long long  zombieId;
};

struct cs_packet_jump {
    unsigned char size;
    char          type;
    float         initVelocity;
};

struct cs_packet_state {
    unsigned char size;
    char          type;
    unsigned char state;
};

struct sc_packet_login_ok {
    unsigned char size;
    char          type;
    long long     playerId;
    Vector3       position;
    int           health;
    float         walkSpeed;
    float         runSpeed;
    int           faintCount;
    bool          isFainted;
};

struct sc_packet_player_info {
    unsigned char size;
    char          type;
    long long     playerId;
    Vector3       position;
    int           health;
    float         walkSpeed;
    float         runSpeed;
    int           faintCount;
    bool          isFainted;
};

struct sc_packet_move {
    unsigned char size;
    char          type;
    long long     playerId;
    Vector3       position;
    Vector3       look;
};

struct sc_packet_attack {
    unsigned char size;
    char          type;
    long long     playerId;
    long long     zombieId;
    Vector3       impactPoint;
};

struct sc_packet_jump {
    unsigned char size;
    char          type;
    long long     playerId;
    float         initVelocity;
};

struct sc_packet_land {
    unsigned char size;
    char          type;
    long long     playerId;
};

struct sc_packet_player_leave {
    unsigned char size;
    char          type;
    long long     playerId;
};

struct sc_packet_game_start {
    unsigned char size;
    char          type;
};

struct sc_packet_snapshot {
    unsigned char size;
    char          type;
    unsigned char count;
    struct Entry {
        long long playerId;
        Vector3   position;
    } entries[0];
};

struct sc_packet_state {
    unsigned char size;
    char          type;
    long long     playerId;
    unsigned char state;
};

struct sc_packet_spawn_zombie {
    unsigned char size;
    char          type;
    long long     zombieId;
    Vector3       position;
    unsigned char zombieType;
};

struct sc_packet_zombie_move {
    unsigned char size;
    char          type;
    long long     zombieId;
    Vector3       position;
    float         dx;
    float         dz;
};

struct sc_packet_zombie_state {
    unsigned char size;
    char          type;
    long long     zombieId;
    unsigned char state;
};

struct sc_packet_zombie_die {
    uint8_t    size;
    char       type;       
    long long  zombieId;   
};
#pragma pack(pop)