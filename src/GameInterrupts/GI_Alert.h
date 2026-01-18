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

    //GI_Alert* setNextAlert(GI_Alert* nextAlert);

private:
    std::string name_;
    //GI_Alert* nextAlert_ = nullptr;
    bool titleText_ = false;
    int numLines;
};