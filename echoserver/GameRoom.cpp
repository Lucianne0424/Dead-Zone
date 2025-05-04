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

    // 1. 웨이브 상태 업데이트
    gameManager.Update();

    // 2. 플레이어 이동 업데이트
    for (auto* p : players) {
        p->posX += p->moveX * p->walkSpeed * dt;
        p->posZ += p->moveZ * p->walkSpeed * dt;
    }

    for (auto* p : players) {
        if (!p->isJumping) continue;

        // 위치 갱신
        p->posY += p->verticalVelocity * dt;
        // 속도 갱신 (중력)
        p->verticalVelocity -= gravity * dt;

        // 착지 체크
        if (p->posY <= groundY) {
            p->posY = groundY;
            p->verticalVelocity = 0.0f;
            p->isJumping = false;

            // 착지 이벤트 전송 (선택 사항)
            // sc_packet_land { size, type=S2C_P_JUMP, playerId }
        }
    }

    // 3. 좀비 스폰 & 이동 업데이트
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
        pkt.playerId = origin->socket;               // origin의 ID
        pkt.position = { origin->posX, origin->posY, origin->posZ };
        pkt.yaw = 0;

        // 모든 peer(=players)에게 이 origin의 상태를 전송
        for (auto* peer : players) {
            PostSendPacket(peer, &pkt, pkt.size);
        }
    }
    //// 4. 상태 메시지 구성 & 브로드캐스트
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
