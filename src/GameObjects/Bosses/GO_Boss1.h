#pragma once

#include "../GameObject.h"
//#include "../globals.h"

class GO_Boss1: public GameObject {
public:
    GO_Boss1(T3DVec3 pos, float rotation);

    static T3DModel* bossModel;
    static uint8_t instanceCount;

    virtual ~GO_Boss1();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    T3DMat4 bossMat;
    T3DMat4FP *bossMatFP;
};