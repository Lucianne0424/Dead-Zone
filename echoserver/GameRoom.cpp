#include "GameRoom.h"
#include "server.h"    
#include <sstream>
#include <algorithm>
#include <chrono>

std::vector<GameRoom*> activeRooms;

GameRoom::GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput)
    : players(playersInput)
{
    activeRooms.push_back(this);
}

GameRoom::~GameRoom() {
    auto it = std::find(activeRooms.begin(), activeRooms.end(), this);
    if (it != activeRooms.end())
        activeRooms.erase(it);
}

void GameRoom::Update(float dt)
{
    // 1. ���̺� ���� ������Ʈ
    static GameManager gameManager;
    gameManager.Update();

    // 2. �÷��̾� �̵� ������Ʈ
    for (auto* p : players) {
        p->posX += p->moveX * p->walkSpeed * dt;
        p->posY += p->moveY * p->walkSpeed * dt;
        p->posZ += p->moveZ * p->walkSpeed * dt;
    }

    // 3. ���� ���� & �̵� ������Ʈ
    static auto lastSpawn = std::chrono::steady_clock::now();
    static std::chrono::milliseconds spawnInterval(1000);
    auto now = std::chrono::steady_clock::now();
    if (zombies.size() < 10 && now - lastSpawn >= spawnInterval) {
        ZombieType t = static_cast<ZombieType>(rand() % 6);
        Zombie z(t);
        z.x = float(rand() % 200);
        z.y = float(rand() % 200);
        zombies.push_back(z);
        lastSpawn = now;
    }
    const float targetX = 100.f, targetY = 100.f;
    for (auto& z : zombies)
        z.UpdatePosition(dt, targetX, targetY);

    // 4. ���� �޽��� ���� & ��ε�ĳ��Ʈ
    std::ostringstream ss;
    ss << "WAVE_STATE: " << gameManager.GetStateString() << "\n"
        << "ZOMBIE_COUNT: " << zombies.size() << "\n";
    for (auto& z : zombies)
        ss << z.GetStatus() << "\n";
    auto msg = ss.str();

    for (auto* p : players) {
        auto* io = new PER_IO_DATA;
        PostSend(p, msg, io);
    }
}
