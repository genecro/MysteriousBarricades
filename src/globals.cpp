#include "globals.h"

namespace global
{
    float elapsedSeconds = 0.0f;
    std::vector<GameInterrupt*>* GameInterruptStack;// = new std::vector<GameInterrupt*>();
    GO_Player* thePlayer;
    GameState* gameState;
    float frameTimeMultiplier;
    surface_t* disp;
    Audio* audioManager;

    //joypad_inputs_t joypad;
    //joypad_buttons_t btn;
    //joypad_buttons_t btnRel;
    //joypad_buttons_t btnHeld;
}