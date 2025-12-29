#pragma once

class GameInterrupt
{
public:
    GameInterrupt();
    virtual ~GameInterrupt();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;
    bool isPauseInterrupt();
    bool isTimeToDestroy();

    GameInterrupt* setNextInterrupt(GameInterrupt* nextInterrupt);

protected:
    bool pauseInterrupt = false;
    bool timeToDestroy = false;
    GameInterrupt* nextInterrupt_ = nullptr;
};