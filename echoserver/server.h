#pragma once

#include <winsock2.h>
#include <windows.h>
#include <string>

#define MAX_BUFFER 8192  // �ӽ� ���� ũ�� Ȯ�� ---- ���Ŀ� �ٲ����

// I/O �۾� ����
enum IO_OPERATION {
    IO_READ,
    IO_WRITE,
    IO_ACCEPT
};

// Ŭ���̾�Ʈ ���� ����
enum SESSION_STATE {
    STATE_LOGIN,  // �α��� ��
    STATE_LOBBY,  // ����(�κ�) ����
    STATE_GAME    // ���� ���� (�÷��̾� ���� ��� ó��)
};

// I/O �۾� ������ ����ü
struct PER_IO_DATA {
    OVERLAPPED overlapped;      // �񵿱� I/O��
    WSABUF wsabuf;              // ���� ����
    char buffer[MAX_BUFFER];    // ���� ������ ���� ����
    IO_OPERATION operationType; // ���� �۾� ����
};

// Ŭ���̾�Ʈ ���� ���ؽ�Ʈ (�÷��̾� ���� ����)
struct PER_SOCKET_CONTEXT {
    SOCKET socket;          // Ŭ���̾�Ʈ ����
    SESSION_STATE state;    // ���� ���� ����
    std::string username;   // �α��� �� �Է��� ����� �̸�

    // �÷��̾� �⺻ ����
    int health;             // ü�� (�ʱ� 100)
    float posX, posY;       // ��ġ (�ʱ� (0,0))
    float walkSpeed;        // �ȴ� �ӵ� (��: 3 m/s)
    float runSpeed;         // �ٴ� �ӵ� (��: 5 m/s)
    int faintCount;         // ���� Ƚ�� (�ִ� 1ȸ ���)
    bool isFainted;         // ���� ���� ����

    // �ð� ��� �̵��� ���� ���� �̵� ����
    // (��: moveX, moveY�� -1 ~ +1 ������ ���̸� �� �������� ������ �̵�)
    float moveX, moveY;
};

// �񵿱� ���� �Լ� ���� (server.cpp���� ����)
void PostSend(PER_SOCKET_CONTEXT* pContext, const std::string& msg, PER_IO_DATA* pIoData);
