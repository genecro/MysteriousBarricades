#pragma once
#include <libdragon.h>
#include <vector>
#include <map>
#include "GameInterrupts/GameInterrupt.h"
#include "GameObjects/GO_Player.h"
#include "GameStates/GameState.h"


#define INTERACTION_ANGLE T3D_PI/6.0f

namespace global
{
    enum {
        CURSOR_STATE_BASE = 0,
        CURSOR_STATE_REPAIR = 1,
        CURSOR_STATE_BARRICADE = 2
    };

    extern float elapsedSeconds;
    extern std::vector<GameInterrupt*>* GameInterruptStack;
    extern GO_Player* thePlayer;
    extern GameState* gameState;
    //extern float usPerFrame;
    //const extern float US_60FPS;
    extern float frameTimeMultiplier;
    extern surface_t* disp;
    //extern uint16_t frameStart;
    //extern joypad_inputs_t input;
}