#pragma once

#include "GO_NPC.h"

class GO_NPCKnight : public GO_NPC {
public:
    GO_NPCKnight(std::string name, T3DVec3 position, float rotation);
    virtual ~GO_NPCKnight();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    static T3DModel* knightModel;
    static uint8_t instanceCount;

private:
    //T3DMat4 knightMat;
    T3DMat4FP *knightMatFP;
    //rspq_block_t *dplKnight;

    T3DSkeleton skel;
    T3DSkeleton skelBlend;
    T3DAnim animIdle;
    T3DAnim animWalk;
    float animBlend = 0.0f;
    int frameIdx = 0;
};