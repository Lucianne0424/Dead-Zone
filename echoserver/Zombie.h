// Zombie.h
#pragma once
#include <cmath>
#include <string>
#include <cstdio>

// ���� ���� ������
enum ZombieType {
    BASIC,      // �⺻ ����
    RUNNING,    // �ٴ� ����
    ELITE,      // ����Ʈ ����
    CHARGER,    // ���� ����
    BOOMER,     // �θ� ����
    HUNTER,     // ���� ����
    BOSS        // ���� ����
};

// ���� ������ ���� ������ (���� ���񿡸� �ش�)
enum BossState {
    BOSS_NORMAL,      // �Ϲݻ���
    BOSS_PREAWAKEN,   // �����غ���� (15�ʰ�, �ִ� HP�� 20% ��ȣ�� ����)
    BOSS_AWAKENED     // �������� (ü�� 250000, ���ݷ� 100, ���ݼӵ� 1.6ȸ/s, �̵��ӵ� 4.5m/s, �޴� ������ 15% ����)
};

class Zombie {
public:
    ZombieType type;
    int health;
    int attack;
    float attackSpeed; // �ʴ� ���� Ƚ��
    float walkSpeed;
    float runSpeed;
    std::string specialSkill;

    // ���� ���񿡸� ���
    BossState bossState;

    // ���� ��ġ
    float x, y, z;

    // ������: �Էµ� Ÿ�Կ� ���� ���� �ʱ�ȭ
    Zombie(ZombieType t) : type(t), x(0.0f), y(0.0f), z(0.0f) {
        if (t == BASIC) {
            health = 100;
            attack = 10;
            attackSpeed = 1.0f;
            walkSpeed = 1.0f;
            runSpeed = 3.3f;
            specialSkill = "None";
        }
        else if (t == RUNNING) {
            health = 100;
            attack = 12;
            attackSpeed = 1.0f;
            walkSpeed = 1.0f;
            runSpeed = 5.0f;
            specialSkill = "None";
        }
        else if (t == ELITE) {
            health = 250;
            attack = 15;
            attackSpeed = 1.1f;
            walkSpeed = 1.1f;
            runSpeed = 3.7f;
            specialSkill = "None";
        }
        else if (t == CHARGER) {
            health = 1500;
            attack = 30;
            attackSpeed = 1.0f;
            // ���� ����� ������ '����' ��ų�� ������, �̵��ӵ��� 3m/s�� ����
            walkSpeed = 3.0f;
            runSpeed = 3.0f;
            specialSkill = "Charge";
        }
        else if (t == BOOMER) {
            health = 3000;
            attack = 30;
            attackSpeed = 0.8f;
            walkSpeed = 2.0f;
            runSpeed = 2.0f;
            specialSkill = "Breath";
        }
        else if (t == HUNTER) {
            health = 300;
            attack = 20;
            attackSpeed = 1.5f;
            walkSpeed = 4.5f;
            runSpeed = 4.5f;
            specialSkill = "Leap";
        }
        else if (t == BOSS) {
            // ���� ����� �ʱ⿡�� �Ϲ� ���·� ����
            bossState = BOSS_NORMAL;
            health = 500000;
            attack = 50;
            attackSpeed = 0.8f;
            walkSpeed = 3.0f;
            runSpeed = 3.0f;
            specialSkill = "Boss Basic Skill";
        }
    }

    // ���� ���� ����: ���� ��ȯ �Լ� (���� ���ǿ� ���� ȣ��)
    void TransitionBossState() {
        if (type == BOSS) {
            if (bossState == BOSS_NORMAL) {
                bossState = BOSS_PREAWAKEN;
                specialSkill = "Pre-awaken: Shield active (20% max HP)";
            }
            else if (bossState == BOSS_PREAWAKEN) {
                bossState = BOSS_AWAKENED;
                health = 250000;
                attack = 100;
                attackSpeed = 1.6f;
                runSpeed = 4.5f;
                specialSkill = "Awakened: Damage reduction 15%";
            }
        }
    }

    // dt: ��� �ð�(��), (targetX, targetY): ��ǥ ��ġ
    // ��, ����� �ȱ� �ӵ��� ����Ͽ� �̵� ����ai �߰� �� �÷��̾ ���� �پ���� �ӵ� �߰�
    void UpdatePosition(float dt, float targetX, float targetY) {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance > 0.001f) {
            float vx = (dx / distance) * walkSpeed;
            float vy = (dy / distance) * walkSpeed;
            x += vx * dt;
            y += vy * dt;
        }
    }

    // ���� ������ ���ڿ��� ��ȯ (�׽�Ʈ �޽���)
    std::string GetStatus() {
        char buffer[512];
        if (type == BOSS) {
            snprintf(buffer, sizeof(buffer),
                "Type: %d (Boss State: %d), HP: %d, ATK: %d, ATK Speed: %.2f, Speed: %.2f, Special: %s, Pos: (%.2f, %.2f)",
                type, bossState, health, attack, attackSpeed, walkSpeed, specialSkill.c_str(), x, y);
        }
        else {
            snprintf(buffer, sizeof(buffer),
                "Type: %d, HP: %d, ATK: %d, ATK Speed: %.2f, Walk: %.2f, Run: %.2f, Special: %s, Pos: (%.2f, %.2f)",
                type, health, attack, attackSpeed, walkSpeed, runSpeed, specialSkill.c_str(), x, y);
        }
        return std::string(buffer);
    }

    // ��ų ��� �Լ� ����
    std::string UseSkill() {
        return "Using skill: " + specialSkill;
    }
};
