#include "globals.h"

namespace global
{
    float elapsedSeconds = 0.0f;
    std::vector<GameInterrupt*>* GameInterruptStack;// = new std::vector<GameInterrupt*>();
    GO_Player* thePlayer;
    GameState* gameState;
    float usPerFrame = 16700.0f;
    const float US_60FPS = 16700.0f;
    float frameTimeMultiplier;
    surface_t* disp;
    uint16_t frameStart;
    //joypad_inputs_t input;
}