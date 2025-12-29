#include "GameInterrupt.h"
#include "../globals.h"

GameInterrupt::GameInterrupt()
{
    
}

GameInterrupt::~GameInterrupt()
{
    if(nextInterrupt_) {
        global::GameInterruptStack->push_back(nextInterrupt_);
    }   
}

bool GameInterrupt::isPauseInterrupt() {
    return pauseInterrupt;
}

bool GameInterrupt::isTimeToDestroy() {
    return timeToDestroy;
}

GameInterrupt* GameInterrupt::setNextInterrupt(GameInterrupt* nextInterrupt) {
    nextInterrupt_ = nextInterrupt;
    return this;
}