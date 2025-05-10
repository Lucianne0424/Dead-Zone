// GameRoom.h
#pragma once

#include "server.h"
#include "GameManager.h"
#include "Zombie.h"
#include <vector>

// Ȱ�� �� ��� ���� ����
class GameRoom;
extern std::vector<GameRoom*> activeRooms;

class GameRoom {
public:
    // ������: playersInput���� �� ��� �ʱ�ȭ, activeRooms�� ���
    GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput);
    ~GameRoom();

    // ƽ �����ٷ����� ȣ���� ������Ʈ �Լ�
    void Update(float dt);

    // �� �濡 ���� �÷��̾��
    std::vector<PER_SOCKET_CONTEXT*> players;

    GameManager              gameManager;
    std::chrono::steady_clock::time_point lastSpawn;
    std::chrono::milliseconds                spawnInterval{ 1000 };

private:
    int   snapshotFrameCount = 0;
    static constexpr int snapshotFrameInterval = 20;
    // �� ���� ���� ����Ʈ
    std::vector<Zombie> zombies;
    long long nextZombieId = 1;
};