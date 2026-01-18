#pragma once
#include <libdragon.h>
#include <vector>
#include <map>
#include <stack>
#include "GameInterrupts/GameInterrupt.h"
#include "GameObjects/GO_Player.h"
#include "GameStates/GameState.h"
#include "Audio.h"


#define INTERACTION_ANGLE T3D_PI/6.0f

namespace global
{
    enum {
        CURSOR_STATE_BASE = 0,
        CURSOR_STATE_REPAIR = 1,
        CURSOR_STATE_BARRICADE = 2
    };

    enum {
        ENEMY_STATE_SEEKING = 0,
        ENEMY_STATE_ATTACKING = 1,
        ENEMY_STATE_CHASING_CURSOR = 2
    };

    extern float elapsedSeconds;
    extern std::vector<GameInterrupt*>* GameInterruptStack;
    //extern GO_Player* thePlayer;
    extern GameState* gameState;
    extern std::stack<GameState*>* GameStateStack;
    extern float frameTimeMultiplier;
    extern surface_t* disp;
    extern Audio* audioManager;

    extern struct gameProgress_t {
        bool training2Unlocked = false;
        bool training3Unlocked = false;
        bool level1Unlocked = true;
        bool level2Unlocked = false;
        bool boss1Unlocked = false;
        uint8_t numBarricades = 3;
        uint8_t rpCapacity = 100;
        float repairSpeedMultiplier = 0.2f;
        bool allTrainingLevelsComplete = false;
        bool trainingRewardReceived = false;
    } gameProgress;

    //extern joypad_inputs_t joypad;
    //extern joypad_buttons_t btn;
    //extern joypad_buttons_t btnRel;
    //extern joypad_buttons_t btnHeld;
}