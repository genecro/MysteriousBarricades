#include "GameInterrupt.h"

GameInterrupt::GameInterrupt()
{
    
}

GameInterrupt::~GameInterrupt()
{
    
}

bool GameInterrupt::isPauseInterrupt() {
    return pauseInterrupt;
}

bool GameInterrupt::isTimeToDestroy() {
    return timeToDestroy;
}