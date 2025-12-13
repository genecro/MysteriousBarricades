#pragma once

#include "GO_Enemy.h"
//#include "../globals.h"

class GO_EnemyBasic: public GO_Enemy {
public:
    GO_EnemyBasic(T3DVec3 pos, T3DVec3 target);

    static T3DModel* enemyModel;
    static uint8_t instanceCount;

    virtual ~GO_EnemyBasic();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    virtual void stun(float stunTimeSeconds) override;

private:
    T3DMat4 enemyMat;
    T3DMat4FP *enemyMatFP;
};