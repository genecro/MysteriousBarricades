#pragma once

#include "GameObject.h"

class GO_Projectile : public GameObject {
public:
    GO_Projectile(T3DVec3 position, float angle, float speed);
    virtual ~GO_Projectile();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    T3DMat4 projectileMat;
    T3DMat4FP* projectileMatFP;
    static T3DModel* projectileModel;

    float angle_;
    float speed_;

    static uint8_t instanceCount;

    color_t objColor_;
};