#pragma once
#include <libdragon.h>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"

class GI_Pause : public GameInterrupt
{
public:
    GI_Pause();
    virtual ~GI_Pause();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    char* pauseStr;
};