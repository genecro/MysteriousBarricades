#pragma once
#include <libdragon.h>
#include <string>
#include <functional>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"
//#include "../GameStates/GameState.h"

class GI_Action : public GameInterrupt
{
public:
    GI_Action(std::function<void ()> theFunction);
    virtual ~GI_Action();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    std::function<void ()> theFunction_;
};