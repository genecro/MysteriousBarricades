#pragma once

#include "GameObject.h"

class GO_RepairBoostInf : public GameObject {
public:
    GO_RepairBoostInf(T3DVec3 position);
    virtual ~GO_RepairBoostInf();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    void consumeEffect() override;

private:
    T3DMat4 repairMat;
    T3DMat4FP* repairMatFP;
    static T3DModel* repairModel;

    static uint8_t instanceCount;
};