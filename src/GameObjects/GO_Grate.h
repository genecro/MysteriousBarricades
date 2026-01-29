#pragma once

#include "GameObject.h"

class GO_Grate : public GameObject {
public:
    GO_Grate(T3DVec3 position, float rotation);
    virtual ~GO_Grate();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    void activate() override;

private:
    T3DMat4 grateMat;
    T3DMat4FP* grateMatFP;
    static T3DModel* grateModel;

    static uint8_t instanceCount;

    float grateAmount_ = 10.0f;

    float targetY_;
    float grateHeight_;
};