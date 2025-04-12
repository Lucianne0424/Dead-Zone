#pragma once
#include <chrono>
#include <string>

// 웨이브 상태 열거형
enum WaveState {
    UG_WAVE1,     // 지하 웨이브 1
    UG_WAVE2,     // 지하 웨이브 2
    UG_WAVE3,     // 지하 웨이브 3
    TRANSITION,   // 지상으로 이동 준비
    AG_WAVE1,     // 지상 웨이브 1
    GAME_OVER     // 게임 종료
};

class GameManager {
public:
    WaveState currentState;
    std::chrono::steady_clock::time_point waveStartTime;
    int waveDuration; // 초 단위, 테스트용으로 짧게 설정

    GameManager()
        : currentState(UG_WAVE1), waveDuration(10) 
    {
        waveStartTime = std::chrono::steady_clock::now();
    }

    void Update() {
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - waveStartTime).count();
        if (elapsed >= waveDuration) {
            TransitionState();
        }
    }

    void TransitionState() {
        switch (currentState) {
        case UG_WAVE1:
            currentState = UG_WAVE2;
            waveDuration = 10; // 테스트용 
            break;
        case UG_WAVE2:
            currentState = UG_WAVE3;
            waveDuration = 10;
            break;
        case UG_WAVE3:
            currentState = TRANSITION;
            waveDuration = 5; 
            break;
        case TRANSITION:
            currentState = AG_WAVE1;
            waveDuration = 15; // 지상 웨이브 1은 15초 (실제 게임에서는 6분)
            break;
        case AG_WAVE1:
            currentState = GAME_OVER;
            break;
        case GAME_OVER:
            break;
        }
        waveStartTime = std::chrono::steady_clock::now();
    }

    std::string GetStateString() {
        switch (currentState) {
        case UG_WAVE1: return "UNDERGROUND WAVE 1";
        case UG_WAVE2: return "UNDERGROUND WAVE 2";
        case UG_WAVE3: return "UNDERGROUND WAVE 3";
        case TRANSITION: return "TRANSITION: OPENING PATH TO ABOVEGROUND";
        case AG_WAVE1: return "ABOVEGROUND WAVE 1";
        case GAME_OVER: return "GAME OVER";
        }
        return "UNKNOWN";
    }
};
