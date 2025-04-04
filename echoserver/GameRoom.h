#pragma once
#include "server.h"      
#include "GameManager.h" 
#include "Zombie.h"      
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <sstream>
#include <cstdlib>       

class GameRoom {
public:
    std::vector<PER_SOCKET_CONTEXT*> players;  // 이 게임룸에 속한 플레이어들
    std::atomic<bool> running;                 // 게임 루프 종료 플래그
    std::thread gameLoopThread;                // 게임 루프를 수행할 스레드
    std::vector<Zombie> zombies;               // 현재 게임룸 내의 좀비들

    // 생성자: 매칭된 플레이어 리스트를 받아 게임 루프 시작
    GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput)
        : players(playersInput), running(true)
    {
        gameLoopThread = std::thread(&GameRoom::GameLoop, this);
    }

    // 소멸자: 게임 루프 종료 및 스레드 정리
    ~GameRoom() {
        running = false;
        if (gameLoopThread.joinable())
            gameLoopThread.join();
    }

    // 게임 루프: 웨이브 진행, 좀비 스폰 및 업데이트, 플레이어 시간 기반 이동, 클라이언트 동기화 처리
    void GameLoop() {
        GameManager gameManager;  // 웨이브 진행 관리 객체
        auto lastSpawnTime = std::chrono::steady_clock::now();
        std::chrono::milliseconds spawnInterval(1000); // 1초마다 좀비 스폰 시도
        const float dt = 0.05f;  // 50ms 업데이트 주기

        while (running) {
            auto now = std::chrono::steady_clock::now();

            // 1. 웨이브 상태 업데이트
            gameManager.Update();

            // 2. 플레이어 이동 업데이트: 각 플레이어의 moveX, moveY에 따라 위치를 갱신
            for (auto player : players) {
                player->posX += player->moveX * player->walkSpeed * dt;
                player->posY += player->moveY * player->walkSpeed * dt;
            }

            // 3. 좀비 스폰 로직: 최대 10마리까지, spawnInterval마다 랜덤 타입의 좀비 생성
            if (zombies.size() < 10 && (now - lastSpawnTime) >= spawnInterval) {
                // 기본적으로 0~5까지의 좀비 타입 (BOSS 제외) 스폰
                ZombieType zType = static_cast<ZombieType>(rand() % 6);
                Zombie z(zType);
                // 스폰 위치를 0~199 범위 내의 랜덤 값으로 설정
                z.x = static_cast<float>(rand() % 200);
                z.y = static_cast<float>(rand() % 200);
                zombies.push_back(z);
                lastSpawnTime = now;
            }

            // 4. 좀비 업데이트: 각 좀비가 고정 타겟 (예 (100, 100)) 쪽으로 이동  --- 추후 플레이어를 향해
            float targetX = 100.0f;
            float targetY = 100.0f;
            for (auto& zombie : zombies) {
                zombie.UpdatePosition(dt, targetX, targetY);
            }

            // 5. 상태 메시지 구성: 웨이브 상태 및 모든 좀비 상태 정보 포함 테스트
            std::stringstream ss;
            ss << "WAVE_STATE: " << gameManager.GetStateString() << "\n";
            ss << "ZOMBIE_COUNT: " << zombies.size() << "\n";
            for (size_t i = 0; i < zombies.size(); i++) {
                ss << zombies[i].GetStatus() << "\n";
            }
            std::string updateMsg = ss.str();

            // 6. 모든 플레이어에게 업데이트 메시지 전송 테스트
            for (auto player : players) {
                PER_IO_DATA* updateData = new PER_IO_DATA;
                PostSend(player, updateMsg, updateData);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
};
