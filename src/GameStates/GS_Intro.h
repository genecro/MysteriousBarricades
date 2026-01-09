#pragma once
#include <libdragon.h>
//#include "GS_MainMenu.h"
#include "GameState.h"

#define INTRO_STEP_TIME 2500.0f
#define INTRO_FADE_TIME 1000.0f
#define NUM_INTRO_SPRITES 3

class GS_Intro : public GameState
{
public:
    GS_Intro();
    ~GS_Intro();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    bool firstUpdate = true;
    float m_timeElapsed = 0;
    const char* sprites[NUM_INTRO_SPRITES] = {"rom:/libdragon.sprite", "rom:/tiny3d.sprite", "rom:/VCI.sprite"};
    uint64_t start_time;
    int i;
    float timeline;
    sprite_t *currSprite;
    rdpq_blitparms_t sprParams = {};
    bool fadingIn, fadingOut;
    float spriteAlpha;
};