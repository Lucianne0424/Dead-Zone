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

constexpr float PLAYER_RADIUS = 50.0f;
constexpr float ZOMBIE_RADIUS = 50.0f;

// 감지 및 공격 반경
constexpr float DETECT_RADIUS = 500.0f;
constexpr float ATTACK_RADIUS = 100.0f;
const float   DETECT_RADIUS2 = DETECT_RADIUS * DETECT_RADIUS;
const float   ATTACK_RADIUS2 = ATTACK_RADIUS * ATTACK_RADIUS;

std::vector<GameRoom*> activeRooms;

GameRoom::GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput)
    : players(playersInput)
{
    activeRooms.push_back(this);
}

GameRoom::~GameRoom()
{
    auto it = std::find(activeRooms.begin(), activeRooms.end(), this);
    if (it != activeRooms.end())
        activeRooms.erase(it);
}

void GameRoom::Update(float dt)
{
    HandlePlayerPhysics(dt);
    HandlePlayerCollisions();
    SpawnZombies();
    UpdateZombies(dt);
    BroadcastSnapshots();
}

void GameRoom::HandlePlayerPhysics(float dt)
{
    const float gravity = 9.8f;
    const float groundY = MAP_MIN_Y;

    for (auto* p : players) {
        p->posX += p->moveX * p->walkSpeed * dt;
        p->posZ += p->moveZ * p->walkSpeed * dt;

        if (p->isJumping) {
            p->posY += p->verticalVelocity * dt;
            p->verticalVelocity -= gravity * dt;

            if (p->posY <= groundY) {
                p->posY = groundY;
                p->verticalVelocity = 0.0f;
                p->isJumping = false;
                SendLandPacket(p);
            }
            else if (p->posY < groundY) {
                p->posY = groundY;
            }
        }
        else if (p->posY < groundY) {
            p->posY = groundY;
        }
    }
}

void GameRoom::SendLandPacket(PER_SOCKET_CONTEXT* p)
{
    sc_packet_land landPkt;
    landPkt.size = sizeof(landPkt);
    landPkt.type = S2C_P_LAND;
    landPkt.playerId = p->socket;
    for (auto* peer : players) {
        if (peer != p)
            PostSendPacket(peer, &landPkt, landPkt.size);
    }
}

void GameRoom::HandlePlayerCollisions()
{
    // 맵 경계 AABB
    const float groundY = MAP_MIN_Y;
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

    // 플레이어 간 구 충돌
    size_t n = players.size();
    for (size_t i = 0; i < n; ++i) {
        auto* a = players[i];
        for (size_t j = i + 1; j < n; ++j) {
            auto* b = players[j];
            ResolveSphereCollision(a->posX, a->posY, a->posZ,
                b->posX, b->posY, b->posZ,
                PLAYER_RADIUS);
        }
    }
}

void GameRoom::ResolveSphereCollision(float& ax, float& ay, float& az,
    float& bx, float& by, float& bz,
    float radius)
{
    float dx = bx - ax;
    float dy = by - ay;
    float dz = bz - az;
    float dist2 = dx * dx + dy * dy + dz * dz;
    float minD = radius * 2.0f;
    if (dist2 < minD * minD && dist2 > 1e-6f) {
        float dist = std::sqrt(dist2);
        float pen = minD - dist;
        float nx = dx / dist;
        float ny = dy / dist;
        float nz = dz / dist;
        float half = pen * 0.5f;
        ax -= nx * half; ay -= ny * half; az -= nz * half;
        bx += nx * half; by += ny * half; bz += nz * half;
    }
}

void GameRoom::SpawnZombies()
{
    auto now = std::chrono::steady_clock::now();
    if (zombies.size() >= 10 || now - lastSpawn < spawnInterval)
        return;

    float xRange = (MAP_MAX_X - PLAYER_RADIUS) - (MAP_MIN_X + PLAYER_RADIUS);
    float zRange = (MAP_MAX_Z - PLAYER_RADIUS) - (MAP_MIN_Z + PLAYER_RADIUS);
    float spawnX = MAP_MIN_X + PLAYER_RADIUS + (static_cast<float>(rand()) / RAND_MAX) * xRange;
    float spawnZ = MAP_MIN_Z + PLAYER_RADIUS + (static_cast<float>(rand()) / RAND_MAX) * zRange;
    float spawnY = MAP_MIN_Y;

    Zombie z(ZombieType::BASIC);
    z.x = spawnX; z.y = spawnY; z.z = spawnZ;
    z.id = nextZombieId++;
    zombies.push_back(z);

    lastSpawn = now;
    sc_packet_spawn_zombie pkt{};
    pkt.size = sizeof(pkt);
    pkt.type = S2C_P_SPAWN_ZOMBIE;
    pkt.zombieId = z.id;
    pkt.position = { z.x, z.y, z.z };
    pkt.zombieType = static_cast<unsigned char>(ZombieType::BASIC);
    for (auto* peer : players)
        PostSendPacket(peer, &pkt, pkt.size);
}

void GameRoom::UpdateZombies(float dt)
{
    for (auto& z : zombies) {
        // 가장 가까운 플레이어 찾기
        PER_SOCKET_CONTEXT* nearest = nullptr;
        float bestDist2 = std::numeric_limits<float>::infinity();
        for (auto* p : players) {
            float dx = p->posX - z.x;
            float dz = p->posZ - z.z;
            float d2 = dx * dx + dz * dz;
            if (d2 < bestDist2) { bestDist2 = d2; nearest = p; }
        }

        // 상태 및 동작 로직
        if (nearest && bestDist2 <= ATTACK_RADIUS2) {
            SetZombieState(z, Zombie::ATTACK);
        }
        else if (nearest && bestDist2 <= DETECT_RADIUS2) {
            SetZombieState(z, Zombie::WALK);
            auto [dx, dz] = z.UpdatePosition(dt, nearest->posX, nearest->posZ);
            BroadcastZombieMove(z, dx, dz);
        }
        else {
            SetZombieState(z, Zombie::IDLE);
        }

        // 맵 경계 클램프
        ClampZombiePosition(z);
    }
}

void GameRoom::SetZombieState(Zombie& z, Zombie::ZOMBIE_STATE newState)
{
    if (z.state == newState) return;
    z.state = newState;
    sc_packet_zombie_state stPkt{};
    stPkt.size = sizeof(stPkt);
    stPkt.type = S2C_P_ZOMBIE_STATE;
    stPkt.zombieId = z.id;
    stPkt.state = static_cast<uint8_t>(z.state);
    for (auto* peer : players)
        PostSendPacket(peer, &stPkt, stPkt.size);
}

void GameRoom::BroadcastZombieMove(const Zombie& z, float dx, float dz)
{
    sc_packet_zombie_move mvPkt{};
    mvPkt.size = static_cast<unsigned char>(sizeof(mvPkt));
    mvPkt.type = S2C_P_ZOMBIE_MOVE;
    mvPkt.zombieId = z.id;
    mvPkt.position = { z.x, z.y, z.z };
    mvPkt.dx = dx;
    mvPkt.dz = dz;
    for (auto* peer : players)
        PostSendPacket(peer, &mvPkt, mvPkt.size);
}

void GameRoom::ClampZombiePosition(Zombie& z)
{
    if (z.x - ZOMBIE_RADIUS < MAP_MIN_X)  z.x = MAP_MIN_X + ZOMBIE_RADIUS;
    else if (z.x + ZOMBIE_RADIUS > MAP_MAX_X) z.x = MAP_MAX_X - ZOMBIE_RADIUS;
    if (z.z - ZOMBIE_RADIUS < MAP_MIN_Z)  z.z = MAP_MIN_Z + ZOMBIE_RADIUS;
    else if (z.z + ZOMBIE_RADIUS > MAP_MAX_Z) z.z = MAP_MAX_Z - ZOMBIE_RADIUS;
    if (z.y < MAP_MIN_Y) z.y = MAP_MIN_Y;
    else if (z.y > MAP_MAX_Y) z.y = MAP_MAX_Y;
}

void GameRoom::BroadcastSnapshots()
{
    if (++snapshotFrameCount < snapshotFrameInterval) return;

    uint8_t count = static_cast<uint8_t>(players.size());
    size_t headerSize = offsetof(sc_packet_snapshot, entries);
    size_t entrySize = sizeof(sc_packet_snapshot::Entry);
    size_t totalSize = headerSize + count * entrySize;

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

void GameRoom::RemoveZombieById(long long zombieId)
{
    auto it = std::find_if(zombies.begin(), zombies.end(),
        [zombieId](const Zombie& z) {
            return z.id == zombieId;
        });
    if (it != zombies.end()) {
        zombies.erase(it);
    }
}
