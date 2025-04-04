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
    std::vector<PER_SOCKET_CONTEXT*> players;  // �� ���ӷ뿡 ���� �÷��̾��
    std::atomic<bool> running;                 // ���� ���� ���� �÷���
    std::thread gameLoopThread;                // ���� ������ ������ ������
    std::vector<Zombie> zombies;               // ���� ���ӷ� ���� �����

    // ������: ��Ī�� �÷��̾� ����Ʈ�� �޾� ���� ���� ����
    GameRoom(const std::vector<PER_SOCKET_CONTEXT*>& playersInput)
        : players(playersInput), running(true)
    {
        gameLoopThread = std::thread(&GameRoom::GameLoop, this);
    }

    // �Ҹ���: ���� ���� ���� �� ������ ����
    ~GameRoom() {
        running = false;
        if (gameLoopThread.joinable())
            gameLoopThread.join();
    }

    // ���� ����: ���̺� ����, ���� ���� �� ������Ʈ, �÷��̾� �ð� ��� �̵�, Ŭ���̾�Ʈ ����ȭ ó��
    void GameLoop() {
        GameManager gameManager;  // ���̺� ���� ���� ��ü
        auto lastSpawnTime = std::chrono::steady_clock::now();
        std::chrono::milliseconds spawnInterval(1000); // 1�ʸ��� ���� ���� �õ�
        const float dt = 0.05f;  // 50ms ������Ʈ �ֱ�

        while (running) {
            auto now = std::chrono::steady_clock::now();

            // 1. ���̺� ���� ������Ʈ
            gameManager.Update();

            // 2. �÷��̾� �̵� ������Ʈ: �� �÷��̾��� moveX, moveY�� ���� ��ġ�� ����
            for (auto player : players) {
                player->posX += player->moveX * player->walkSpeed * dt;
                player->posY += player->moveY * player->walkSpeed * dt;
            }

            // 3. ���� ���� ����: �ִ� 10��������, spawnInterval���� ���� Ÿ���� ���� ����
            if (zombies.size() < 10 && (now - lastSpawnTime) >= spawnInterval) {
                // �⺻������ 0~5������ ���� Ÿ�� (BOSS ����) ����
                ZombieType zType = static_cast<ZombieType>(rand() % 6);
                Zombie z(zType);
                // ���� ��ġ�� 0~199 ���� ���� ���� ������ ����
                z.x = static_cast<float>(rand() % 200);
                z.y = static_cast<float>(rand() % 200);
                zombies.push_back(z);
                lastSpawnTime = now;
            }

            // 4. ���� ������Ʈ: �� ���� ���� Ÿ�� (�� (100, 100)) ������ �̵�  --- ���� �÷��̾ ����
            float targetX = 100.0f;
            float targetY = 100.0f;
            for (auto& zombie : zombies) {
                zombie.UpdatePosition(dt, targetX, targetY);
            }

            // 5. ���� �޽��� ����: ���̺� ���� �� ��� ���� ���� ���� ���� �׽�Ʈ
            std::stringstream ss;
            ss << "WAVE_STATE: " << gameManager.GetStateString() << "\n";
            ss << "ZOMBIE_COUNT: " << zombies.size() << "\n";
            for (size_t i = 0; i < zombies.size(); i++) {
                ss << zombies[i].GetStatus() << "\n";
            }
            std::string updateMsg = ss.str();

            // 6. ��� �÷��̾�� ������Ʈ �޽��� ���� �׽�Ʈ
            for (auto player : players) {
                PER_IO_DATA* updateData = new PER_IO_DATA;
                PostSend(player, updateMsg, updateData);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
};
