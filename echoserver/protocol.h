#pragma once

// ���� �������� �����
constexpr char S2C_P_PLAYER_INFO = 1;  // ���� �� Ŭ���̾�Ʈ: �÷��̾� ���� ������Ʈ (�α���, ���� ����ȭ)
constexpr char S2C_P_MOVE = 2;  // ���� �� Ŭ���̾�Ʈ: �̵� ������Ʈ (��ġ broadcast)
constexpr char S2C_P_ATTACK = 3;  // ���� �� Ŭ���̾�Ʈ: ���� �̺�Ʈ broadcast (���� ���� ���)
constexpr char S2C_P_GAME_STATE = 4;  // ���� �� Ŭ���̾�Ʈ: ���� ���� ������Ʈ
constexpr char C2S_P_LOGIN = 5;  // Ŭ���̾�Ʈ �� ����: �α��� ��û
constexpr char C2S_P_MOVE = 6;  // Ŭ���̾�Ʈ �� ����: �̵� ��û
constexpr char C2S_P_ATTACK = 7;  // Ŭ���̾�Ʈ �� ����: ���� ��û

// ���� �߰�: �÷��̾� ���� �˸� (��ε�ĳ��Ʈ)
constexpr char S2C_P_PLAYER_LEAVE = 11;

// ���� �߰�: ���� ���� �̺�Ʈ (��ε�ĳ��Ʈ)
constexpr char S2C_P_GAME_START = 12;

constexpr char MAX_ID_LEN = 20;

struct Vector3 {
    float x;
    float y;
    float z;
};

#pragma pack(push, 1)

// Ŭ���̾�Ʈ �� ���� �α��� ��û ��Ŷ
struct cs_packet_login {
    unsigned char size;              // ��ü ��Ŷ ũ��
    char type;                       // �������� Ÿ��: C2S_P_LOGIN
    char username[MAX_ID_LEN];       // ����ڸ�
};

// ���� �� Ŭ���̾�Ʈ �α��� ���� �� �÷��̾� �ʱ� ���� ��Ŷ
struct sc_packet_login_ok {
    unsigned char size;              // ��ü ��Ŷ ũ��
    char type;                       // �������� Ÿ��: S2C_P_PLAYER_INFO
    long long playerId;              // �÷��̾� ���� ID
    Vector3 position;                // �ʱ� ��ġ (x, y, z)
    int health;                      // ü�� 
    float walkSpeed;                 // �ȴ� �ӵ� (3 m/s)
    float runSpeed;                  // �ٴ� �ӵ� (5 m/s)
    int faintCount;                  // ���� Ƚ�� (�ִ� 1ȸ ���)
    bool isFainted;                  // ���� ���� ����
};

// Ŭ���̾�Ʈ �� ���� �̵� ��û ��Ŷ (Ȯ�� ����: ȸ�� ���� ����)
struct cs_packet_move {
    unsigned char size;      // ��ü ��Ŷ ũ��
    char type;               // �������� Ÿ��: C2S_P_MOVE
    Vector3 direction;       // �̵� ����
    float yaw;               // �¿� ȸ�� ���� 
};

// ���� �� Ŭ���̾�Ʈ �̵� ������Ʈ ��Ŷ
struct sc_packet_move {
    unsigned char size;      // ��ü ��Ŷ ũ��
    char type;               // �������� Ÿ��: S2C_P_MOVE
    long long playerId;      // �̵��� �÷��̾��� ID
    Vector3 position;        // ���ο� ��ġ (x, y, z)
    float yaw;               // ������Ʈ�� ȸ�� ����
};

// Ŭ���̾�Ʈ �� ���� ���� ��û ��Ŷ
struct cs_packet_attack {
    unsigned char size;              // ��ü ��Ŷ ũ��
    char type;                       // �������� Ÿ��: C2S_P_ATTACK
    Vector3 attackDirection;         // ���� ���� (�ѱ� ���� Ȥ�� ���� ���� ����)
    // �߰� ���� (���� ����) �߰� 
};

// ���� �� Ŭ���̾�Ʈ ���� �̺�Ʈ ��Ŷ
struct sc_packet_attack {
    unsigned char size;              // ��ü ��Ŷ ũ��
    char type;                       // �������� Ÿ��: S2C_P_ATTACK
    long long playerId;              // ������ ������ �÷��̾��� ID
    long long zombieId;              // ���� ���� ������ ID
    Vector3 impactPoint;             // ������ ������ ��ġ (x, y, z)
};

// ���� �� Ŭ���̾�Ʈ �÷��̾� ���� �˸� ��Ŷ
struct sc_packet_player_leave {
    unsigned char size;  // ��ü ��Ŷ ũ��
    char type;           // �������� Ÿ��: S2C_P_PLAYER_LEAVE
    long long playerId;  // ������ �÷��̾��� ID
};

// ���� �� Ŭ���̾�Ʈ ���� ���� �̺�Ʈ ��Ŷ
struct sc_packet_game_start {
    unsigned char size;  // ��ü ��Ŷ ũ��
    char type;           // �������� Ÿ��: S2C_P_GAME_START
};

#pragma pack(pop)
