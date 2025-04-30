#pragma once
#include <chrono>
#include <string>

enum WaveState {
    UG_WAVE1, UG_WAVE2, UG_WAVE3,
    TRANSITION, AG_WAVE1, GAME_OVER
};

class GameManager {
public:
    WaveState currentState;
    std::chrono::steady_clock::time_point waveStartTime;
    int waveDuration;

    GameManager()
        : currentState(UG_WAVE1), waveDuration(10) {
        waveStartTime = std::chrono::steady_clock::now();
    }

    void Update() {
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - waveStartTime).count();
        if (elapsed >= waveDuration) TransitionState();
    }

    void TransitionState() {
        switch (currentState) {
        case UG_WAVE1:     currentState = UG_WAVE2; waveDuration = 10; break;
        case UG_WAVE2:     currentState = UG_WAVE3; waveDuration = 10; break;
        case UG_WAVE3:     currentState = TRANSITION; waveDuration = 5; break;
        case TRANSITION:   currentState = AG_WAVE1; waveDuration = 15; break;
        case AG_WAVE1:     currentState = GAME_OVER; break;
        case GAME_OVER:    break;
        }
        waveStartTime = std::chrono::steady_clock::now();
    }

    std::string GetStateString() {
        switch (currentState) {
        case UG_WAVE1:   return "UNDERGROUND WAVE 1";
        case UG_WAVE2:   return "UNDERGROUND WAVE 2";
        case UG_WAVE3:   return "UNDERGROUND WAVE 3";
        case TRANSITION: return "TRANSITION: OPENING PATH TO ABOVEGROUND";
        case AG_WAVE1:   return "ABOVEGROUND WAVE 1";
        case GAME_OVER:  return "GAME OVER";
        }
        return "UNKNOWN";
    }
};
