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

    void RemoveZombieById(long long zombieId);
private:
    std::vector<Zombie>              zombies;
    uint32_t                          snapshotFrameCount = 0;
    uint32_t                          snapshotFrameInterval;
    long long nextZombieId = 1;
    void HandlePlayerPhysics(float dt);
    void HandlePlayerCollisions();
    void ResolveSphereCollision(float& ax, float& ay, float& az,
        float& bx, float& by, float& bz,
        float   radius);
    void SendLandPacket(PER_SOCKET_CONTEXT* p);

    void SpawnZombies();
    void UpdateZombies(float dt);
    void SetZombieState(Zombie& z, Zombie::ZOMBIE_STATE newState);
    void BroadcastZombieMove(const Zombie& z);
    void ClampZombiePosition(Zombie& z);

    void BroadcastSnapshots();
};