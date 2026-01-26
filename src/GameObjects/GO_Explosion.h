#pragma once

#include "GameObject.h"

class GO_Explosion : public GameObject {
public:
    GO_Explosion(T3DVec3 position, float size, color_t color, float lifespan);
    virtual ~GO_Explosion();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    float lifespan_;
    color_t explosionColor_;

private:
    T3DMat4 explosionMat;
    T3DMat4FP* explosionMatFP;
    static T3DModel* explosionModel;

    static uint8_t instanceCount;

    rspq_block_t* dplExplosion;
};