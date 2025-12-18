#pragma once

#include "GO_Barricade.h"
//#include "../globals.h"

class GO_BarricadeStandard: public GO_Barricade {
public:
    GO_BarricadeStandard(T3DVec3 pos, T3DVec3 sizeRotation, color_t objColor);

    static T3DModel* barricadeModel;
    static uint8_t instanceCount;

    virtual ~GO_BarricadeStandard();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    virtual void castSuccess() override;

    void processEnemy(GO_Enemy* theEnemy);

private:
    T3DMat4 barricadeMat;
    T3DMat4FP *barricadeMatFP;
};