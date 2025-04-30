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

private:
    // �� ���� ���� ����Ʈ
    std::vector<Zombie> zombies;
};