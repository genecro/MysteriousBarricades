#pragma once

#include "GO_Repairable.h"

class GO_RepairableTower: public GO_Repairable {
public:
    GO_RepairableTower(T3DVec3 pos, int HPTotal, int HPCurrent, color_t objColor, float repelAngleMin, float repelAngleMax);

    static T3DModel* towerModel;
    static uint8_t instanceCount;

    virtual ~GO_RepairableTower();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    virtual void processProjectile(GO_Projectile* theProjectile);

private:
    T3DMat4 towerMat;
    T3DMat4FP *towerMatFP;
};