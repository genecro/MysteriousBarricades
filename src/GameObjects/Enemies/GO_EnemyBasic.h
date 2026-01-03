#pragma once

#include "GO_Enemy.h"
//#include "../globals.h"

class GO_EnemyBasic: public GO_Enemy {
public:
    GO_EnemyBasic(T3DVec3 pos, GO_Repairable* target, bool dropItem = true);
    GO_EnemyBasic(T3DVec3 pos, T3DVec3 targetPos, bool dropItem = true);

    static T3DModel* enemyModel;
    static uint8_t instanceCount;

    virtual ~GO_EnemyBasic();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    virtual void stun(float stunTimeSeconds) override;
    virtual void attackTarget() override;
    virtual void cursorMakingBarricade(T3DVec3 cursorPos) override;
    virtual void cursorNotMakingBarricade() override;

private:
    T3DMat4 enemyMat;
    T3DMat4FP *enemyMatFP;
};