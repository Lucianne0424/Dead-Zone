// GameRoom.h
#pragma once

#include "server.h"
#include "GameManager.h"
#include "Zombie.h"
#include <vector>

// 활성 방 목록 전방 선언
class GameRoom;
extern std::vector<GameRoom*> activeRooms;

class GameRoom {
public:
    // 생성자: playersInput으로 방 멤버 초기화, activeRooms에 등록
    GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput);
    ~GameRoom();

    // 틱 스케줄러에서 호출할 업데이트 함수
    void Update(float dt);

    // 이 방에 속한 플레이어들
    std::vector<PER_SOCKET_CONTEXT*> players;

    GameManager              gameManager;
    std::chrono::steady_clock::time_point lastSpawn;
    std::chrono::milliseconds                spawnInterval{ 1000 };

private:
    int   snapshotFrameCount = 0;
    static constexpr int snapshotFrameInterval = 20;
    // 방 내부 좀비 리스트
    std::vector<Zombie> zombies;
    long long nextZombieId = 1;
};