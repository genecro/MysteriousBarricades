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

    enum levelLockMasks : uint16_t {
        TRAINING2LOCK   = 0b0000000000000001,
        TRAINING3LOCK   = 0b0000000000000010,
        LEVEL1LOCK      = 0b0000000000000100,
        BOSS1LOCK       = 0b0000000000001000,
        CHALLENGE1LOCK  = 0b0000000000010000
    };

    enum rewardMasks : uint16_t {
        TRAININGREWARD  = 0b0000000000000001,
        LEVEL1REWARD    = 0b0000000000000010,
        BOSS1REWARD     = 0b0000000000000100
    };

    extern struct gameProgress_t {
        uint16_t levelLockStatus = 0;
        uint16_t rewardStatus = 0;
        uint16_t numBarricades = 2;
        uint16_t rpCapacity = 100;
        float repairSpeedMultiplier = 0.2f;
        bool allTrainingLevelsComplete = false;
        bool barricadesCanRicochet = false;
        int challenge1HighScore = 0;
        bool everythingUnlocked = false;
    } gameProgress;

    extern std::string playerName;

    //extern joypad_inputs_t joypad;
    //extern joypad_buttons_t btn;
    //extern joypad_buttons_t btnRel;
    //extern joypad_buttons_t btnHeld;
}