#pragma once

#include "GO_Enemy.h"
//#include "../globals.h"

class GO_EnemyRushToBarricade: public GO_Enemy {
public:
    GO_EnemyRushToBarricade(T3DVec3 pos, GO_Repairable* target, bool dropItem = true);
    GO_EnemyRushToBarricade(T3DVec3 pos, T3DVec3 targetPos, bool dropItem = true);

    static T3DModel* enemyModel;
    static uint8_t instanceCount;

    virtual ~GO_EnemyRushToBarricade();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    virtual void stun(float stunTimeSeconds) override;
    virtual void attackTarget() override;

private:
    T3DMat4 enemyMat;
    T3DMat4FP *enemyMatFP;
};