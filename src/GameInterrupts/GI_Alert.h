#pragma once
#include <libdragon.h>
#include <string>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"
//#include "../GameStates/GameState.h"

class GI_Alert : public GameInterrupt
{
public:
    GI_Alert(std::string name, bool isPause, bool titleText = false);
    virtual ~GI_Alert();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    std::string name_;
    bool titleText_ = false;
    int numLines;
};