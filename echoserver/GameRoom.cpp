#include "GameRoom.h"
#include "server.h"
#include "protocol.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>

constexpr float MAP_MIN_X = 237.0f;
constexpr float MAP_MAX_X = 2030.0f;
constexpr float MAP_MIN_Y = 0.0f;
constexpr float MAP_MAX_Y = 960.0f;
constexpr float MAP_MIN_Z = -3552.0f;
constexpr float MAP_MAX_Z = 3535.0f;

constexpr float PLAYER_RADIUS = 10.0f;

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
    const float groundY = MAP_MIN_Y;

    //  �̵� ���� ���� ó��
    for (auto* p : players) {
        p->posX += p->moveX * p->walkSpeed * dt;
        //p->posY += p->moveY * p->walkSpeed * dt;
        p->posZ += p->moveZ * p->walkSpeed * dt;

        if (p->isJumping) {
            p->posY += p->verticalVelocity * dt;
            p->verticalVelocity -= gravity * dt;

            if (p->posY <= groundY) {
                p->posY = groundY;
                p->verticalVelocity = 0.0f;
                p->isJumping = false;

                sc_packet_land landPkt;
                landPkt.size = sizeof(landPkt);
                landPkt.type = S2C_P_LAND;
                landPkt.playerId = p->socket;
                for (auto* peer : players) {
                    if (peer != p)
                        PostSendPacket(peer, &landPkt, landPkt.size);
                }
            }  
            else {
                if (p->posY < groundY)
                    p->posY = groundY;
            }
        }
        else {
            if (p->posY < groundY)
                p->posY = groundY;
        }
    }  

    // �÷��̾� �� �� �浹 ó�� (AABB Ŭ����)
    for (auto* p : players) {
        if (p->posX - PLAYER_RADIUS < MAP_MIN_X)
            p->posX = MAP_MIN_X + PLAYER_RADIUS;
        else if (p->posX + PLAYER_RADIUS > MAP_MAX_X)
            p->posX = MAP_MAX_X - PLAYER_RADIUS;

        if (p->posZ - PLAYER_RADIUS < MAP_MIN_Z)
            p->posZ = MAP_MIN_Z + PLAYER_RADIUS;
        else if (p->posZ + PLAYER_RADIUS > MAP_MAX_Z)
            p->posZ = MAP_MAX_Z - PLAYER_RADIUS;

        if (p->posY < groundY)
            p->posY = groundY;
    }

    // �÷��̾� �� �÷��̾� �浹 ó�� (Sphere vs Sphere)
    size_t n = players.size();
    for (size_t i = 0; i < n; ++i) {
        auto* a = players[i];
        for (size_t j = i + 1; j < n; ++j) {
            auto* b = players[j];

            float dx = b->posX - a->posX;
            float dy = b->posY - a->posY;
            float dz = b->posZ - a->posZ;
            float dist2 = dx * dx + dy * dy + dz * dz;
            float minD = PLAYER_RADIUS * 2.0f;
            if (dist2 < minD * minD && dist2 > 1e-6f) {
                float dist = std::sqrt(dist2);
                float pen = minD - dist;
    
                float nx = dx / dist;
                float ny = dy / dist;
                float nz = dz / dist;
                float half = pen * 0.5f;

                a->posX -= nx * half;  a->posY -= ny * half;  a->posZ -= nz * half;
                b->posX += nx * half;  b->posY += ny * half;  b->posZ += nz * half;
            }
        }
    }

    // ���� ���� �̵� ������Ʈ
    auto now = std::chrono::steady_clock::now();
    if (zombies.size() < 10 && now - lastSpawn >= spawnInterval) {
        ZombieType t = ZombieType::BASIC;

        float xRange = (MAP_MAX_X - PLAYER_RADIUS) - (MAP_MIN_X + PLAYER_RADIUS);
        float zRange = (MAP_MAX_Z - PLAYER_RADIUS) - (MAP_MIN_Z + PLAYER_RADIUS);

        float spawnX = MAP_MIN_X + PLAYER_RADIUS + (static_cast<float>(rand()) / RAND_MAX) * xRange;
        float spawnZ = MAP_MIN_Z + PLAYER_RADIUS + (static_cast<float>(rand()) / RAND_MAX) * zRange;
        float spawnY = MAP_MIN_Y; 

        Zombie z(t);
        z.x = spawnX;
        z.y = spawnY;
        z.z = spawnZ;

        long long zid = nextZombieId++;
        z.id = zid;
        zombies.push_back(z);

        lastSpawn = now;

        sc_packet_spawn_zombie pkt{};
        pkt.size = static_cast<unsigned char>(sizeof(pkt));
        pkt.type = S2C_P_SPAWN_ZOMBIE;
        pkt.zombieId = zid;
        pkt.position = { z.x, z.y, z.z };
        pkt.zombieType = static_cast<unsigned char>(t);

        for (auto* peer : players)
            PostSendPacket(peer, &pkt, pkt.size);
    }
    
    constexpr float DETECT_RADIUS = 500.0f;
    const float   DETECT_RADIUS2 = DETECT_RADIUS * DETECT_RADIUS;

    for (auto& z : zombies) {
        PER_SOCKET_CONTEXT* nearest = nullptr;
        float bestDist2 = std::numeric_limits<float>::infinity();
        for (auto* p : players) {
            float dx = p->posX - z.x;
            float dz = p->posZ - z.z;
            float d2 = dx * dx + dz * dz;
            if (d2 < bestDist2) {
                bestDist2 = d2;
                nearest = p;
            }
        }
        if (nearest && bestDist2 <= DETECT_RADIUS2) {
            if (z.state != Zombie::WALK) {
                z.state = Zombie::WALK;
                sc_packet_zombie_state stPkt{};
                stPkt.size = sizeof(stPkt);
                stPkt.type = S2C_P_ZOMBIE_STATE;
                stPkt.zombieId = z.id;
                stPkt.state = static_cast<uint8_t>(z.state);
                for (auto* peer : players)
                    PostSendPacket(peer, &stPkt, stPkt.size);
            }
            float tx = nearest->posX;
            float tz = nearest->posZ;
            z.UpdatePosition(dt, tx, tz);

            sc_packet_zombie_move mvPkt{};
            mvPkt.size = static_cast<unsigned char>(sizeof(mvPkt));
            mvPkt.type = S2C_P_ZOMBIE_MOVE;
            mvPkt.zombieId = z.id;
            mvPkt.position = { z.x, z.y, z.z };
            for (auto* peer : players)
                PostSendPacket(peer, &mvPkt, mvPkt.size);
        }
        else {
            if (z.state != Zombie::IDLE) {
                z.state = Zombie::IDLE;
                sc_packet_zombie_state stPkt{};
                stPkt.size = sizeof(stPkt);
                stPkt.type = S2C_P_ZOMBIE_STATE;
                stPkt.zombieId = z.id;
                stPkt.state = static_cast<uint8_t>(z.state);
                for (auto* peer : players)
                    PostSendPacket(peer, &stPkt, stPkt.size);
            }
        }
    }

    if (++snapshotFrameCount >= snapshotFrameInterval) {
        uint8_t count = static_cast<uint8_t>(players.size());
        size_t  headerSize = offsetof(sc_packet_snapshot, entries);
        size_t  entrySize = sizeof(sc_packet_snapshot::Entry);
        size_t  totalSize = headerSize + count * entrySize;

        char* buf = reinterpret_cast<char*>(malloc(totalSize));
        if (!buf) {
            std::cerr << "[Error] snapshot buf alloc failed: " << totalSize << " bytes\n";
            snapshotFrameCount = 0;
            return;
        }

        auto* hdr = reinterpret_cast<sc_packet_snapshot*>(buf);
        hdr->size = static_cast<unsigned char>(totalSize);
        hdr->type = S2C_P_SNAPSHOT;
        hdr->count = count;

        for (int i = 0; i < count; ++i) {
            auto* p = players[i];
            hdr->entries[i].playerId = p->socket;
            hdr->entries[i].position = { p->posX, p->posY, p->posZ };
        }

        for (auto* peer : players)
            PostSendPacket(peer, buf, totalSize);
        free(buf);
        snapshotFrameCount = 0;
    }
}
