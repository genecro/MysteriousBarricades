#pragma once
#include <libdragon.h>
#include <vector>
#include <map>
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
        ENEMY_STATE_ATTACKING = 1
    };

    extern float elapsedSeconds;
    extern std::vector<GameInterrupt*>* GameInterruptStack;
    extern GO_Player* thePlayer;
    extern GameState* gameState;
    extern float frameTimeMultiplier;
    extern surface_t* disp;
    extern Audio* audioManager;

    //extern joypad_inputs_t joypad;
    //extern joypad_buttons_t btn;
    //extern joypad_buttons_t btnRel;
    //extern joypad_buttons_t btnHeld;
}