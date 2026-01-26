#pragma once

#include "../GameObject.h"
#include "../Repairables/GO_Repairable.h"
//#include "../../globals.h"

class GO_Enemy: public GameObject {
public:
    GO_Enemy();

    float HPTotal_;
    float HPCurrent_;
    float height_;
    float intendedRotation_;
    float rotationIncrement_;

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Enemy();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;
    virtual void stun(float stunTimeSeconds);

    void summon();

    void updateHPBar();
    void drawHPBar();
    void setGroundCoord(float newGroundCoord);

    void pushAwayFromBarricade(T3DVec3 sourcePos, float angle, float distance);
    void pushAwayFromRepairable(GO_Repairable* repairable, float distance);
    void receiveDamage(float damageAmount);

    virtual void cursorMakingBarricade(T3DVec3 cursorPos);
    virtual void cursorNotMakingBarricade();

    int enemyState_;

    int getState();
    void setStateAttacking(GO_Repairable* target);
    void setStateSeeking(GO_Repairable* target = nullptr);

    float attackRate;
    float attackDamage;
    float attackAnimDistance;
    T3DVec3 attackAnimOffset{0,0,0};
    virtual void attackTarget();

    virtual void processProjectile(GO_Projectile* theProjectile);

    float HPBarTotalLength_;
    float HPBarCurrentLength_;
    float HPBarHeight_ = 5.0;
    float speed_;
    //float lifetime_ = 0;

    bool isStunned_ = false;
    float stunCooldown_ = 0.0f;

    bool isInvincible_ = false;

    bool displayModel_ = true;

    bool dropItem_ = true;

    bool isMoving_ = true;

    T3DVec3 targetPos_;
    GO_Repairable* target_ = nullptr;

    T3DVec3 HPBarPos_ = (T3DVec3){-2, -2, -2};

protected:
    //rspq_block_t *dplEnemy;

private:
    color_t HPBarColor_ = (color_t){0,0,0,0};

    
};