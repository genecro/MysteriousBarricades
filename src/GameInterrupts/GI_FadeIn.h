#pragma once
#include <libdragon.h>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"
//#include "../GameStates/GameState.h"

class GI_FadeIn : public GameInterrupt
{
public:
    GI_FadeIn();
    GI_FadeIn(float fadeTime);
    virtual ~GI_FadeIn();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    uint64_t startTime_;
    float timeline_;

    bool started_ = false;

    float fadeAlpha_;
    float fadeTime_;
};