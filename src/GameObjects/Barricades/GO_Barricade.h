#pragma once

#include "../GameObject.h"
//#include "../globals.h"

class GO_Barricade: public GameObject {
public:
    GO_Barricade();

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Barricade();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;

    void checkTimeLeft();

    bool timeToDelete = false;

    float framesUntilDelete = 5.0f*60.0f;
};