#pragma once
#include <libdragon.h>
#include <string>
#include "../Fonts.h"
#include "GameInterrupt.h"
//#include "../globals.h"
//#include "../GameStates/GameState.h"

class GI_Dialog : public GameInterrupt
{
public:
    GI_Dialog(int dialogID);
    GI_Dialog(std::string name, std::string text);
    virtual ~GI_Dialog();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    std::string name_;
    std::string text_;
    int currChars=0;
};