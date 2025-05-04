#include "GameRoom.h"
#include "server.h"
#include "protocol.h"
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
    const float gravity = 9.8f;
    const float groundY = 0.0f;

    // 1. ���̺� ���� ������Ʈ
    gameManager.Update();

    // 2. �÷��̾� �̵� ������Ʈ
    for (auto* p : players) {
        p->posX += p->moveX * p->walkSpeed * dt;
        p->posZ += p->moveZ * p->walkSpeed * dt;
    }

    for (auto* p : players) {
        if (!p->isJumping) continue;

        // ��ġ ����
        p->posY += p->verticalVelocity * dt;
        // �ӵ� ���� (�߷�)
        p->verticalVelocity -= gravity * dt;

        // ���� üũ
        if (p->posY <= groundY) {
            p->posY = groundY;
            p->verticalVelocity = 0.0f;
            p->isJumping = false;

            // ���� �̺�Ʈ ���� (���� ����)
            // sc_packet_land { size, type=S2C_P_JUMP, playerId }
        }
    }

    // 3. ���� ���� & �̵� ������Ʈ
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

    for (auto* origin : players) {
        sc_packet_move pkt{};
        pkt.size = sizeof(sc_packet_move);
        pkt.type = S2C_P_MOVE;
        pkt.playerId = origin->socket;               // origin�� ID
        pkt.position = { origin->posX, origin->posY, origin->posZ };
        pkt.yaw = 0;

        // ��� peer(=players)���� �� origin�� ���¸� ����
        for (auto* peer : players) {
            PostSendPacket(peer, &pkt, pkt.size);
        }
    }
    //// 4. ���� �޽��� ���� & ��ε�ĳ��Ʈ
    //std::ostringstream ss;
    //ss << "WAVE_STATE: " << gameManager.GetStateString() << "\n"
    //    << "ZOMBIE_COUNT: " << zombies.size() << "\n";
    //for (auto& z : zombies)
    //    ss << z.GetStatus() << "\n";
    //auto msg = ss.str();

    //for (auto* p : players) {
    //    auto* io = new PER_IO_DATA;
    //    PostSend(p, msg, io);
    //}
}
