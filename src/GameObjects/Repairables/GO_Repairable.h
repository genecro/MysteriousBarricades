#pragma once

#include "../GameObject.h"
//#include "../../globals.h"

class GO_Repairable: public GameObject {
public:
    GO_Repairable();

    int HPTotal_;
    int HPCurrent_;
    float height_;

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Repairable();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;

    void updateHPBar();
    void drawHPBar();

    float HPBarTotalLength_;
    float HPBarCurrentLength_;
    float HPBarHeight_ = 5.0;

    T3DVec3 HPBarPos_ = (T3DVec3){-2, -2, -2};

private:
    color_t HPBarColor_ = (color_t){0,0,0,0};
};