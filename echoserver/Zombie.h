// Zombie.h
#pragma once
#include <cmath>
#include <string>
#include <cstdio>

// 좀비 종류 열거형
enum ZombieType {
    BASIC,      // 기본 좀비
    RUNNING,    // 뛰는 좀비
    ELITE,      // 엘리트 좀비
    CHARGER,    // 차저 좀비
    BOOMER,     // 부머 좀비
    HUNTER,     // 헌터 좀비
    BOSS        // 보스 좀비
};

// 보스 좀비의 상태 열거형 (보스 좀비에만 해당)
enum BossState {
    BOSS_NORMAL,      // 일반상태
    BOSS_PREAWAKEN,   // 각성준비상태 (15초간, 최대 HP의 20% 보호막 생성)
    BOSS_AWAKENED     // 각성상태 (체력 250000, 공격력 100, 공격속도 1.6회/s, 이동속도 4.5m/s, 받는 데미지 15% 감소)
};

class Zombie {
public:
    ZombieType type;
    int health;
    int attack;
    float attackSpeed; // 초당 공격 횟수
    float walkSpeed;
    float runSpeed;
    std::string specialSkill;

    // 보스 좀비에만 사용
    BossState bossState;

    // 현재 위치
    float x, y, z;

    // 생성자: 입력된 타입에 따라 스펙 초기화
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
            // 차저 좀비는 별도로 '돌진' 스킬을 가지며, 이동속도는 3m/s로 고정
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
            // 보스 좀비는 초기에는 일반 상태로 시작
            bossState = BOSS_NORMAL;
            health = 500000;
            attack = 50;
            attackSpeed = 0.8f;
            walkSpeed = 3.0f;
            runSpeed = 3.0f;
            specialSkill = "Boss Basic Skill";
        }
    }

    // 보스 좀비 전용: 상태 전환 함수 (추후 조건에 따라 호출)
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

    // dt: 경과 시간(초), (targetX, targetY): 목표 위치
    // 단, 현재는 걷기 속도를 사용하여 이동 좀비ai 추가 시 플레이어를 향해 뛰어오는 속도 추가
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

    // 상태 정보를 문자열로 반환 (테스트 메시지)
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

    // 스킬 사용 함수 예시
    std::string UseSkill() {
        return "Using skill: " + specialSkill;
    }
};
