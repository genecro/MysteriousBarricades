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

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Enemy();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;
    virtual void stun(float stunTimeSeconds) = 0;

    void updateHPBar();
    void drawHPBar();
    void setGroundCoord(float newGroundCoord);

    void pushAwayFromBarricade(T3DVec3 sourcePos, float angle, float distance);
    void pushAwayFromRepairable(GO_Repairable* repairable, float distance);
    void receiveDamage(float damageAmount);

    int enemyState_;

    int getState();
    void setStateAttacking(GO_Repairable* target);
    void setStateSeeking(GO_Repairable* target = nullptr);

    float attackRate;
    float attackDamage;
    float attackAnimDistance;
    T3DVec3 attackAnimOffset{0,0,0};
    virtual void attackTarget() = 0;

    float HPBarTotalLength_;
    float HPBarCurrentLength_;
    float HPBarHeight_ = 5.0;
    float speed_;
    float lifetime_;

    bool isStunned_;
    float stunCooldown_;

    bool isInvincible_;

    bool displayModel_ = true;

    //T3DVec3 target_;
    GO_Repairable* target_;

    T3DVec3 HPBarPos_ = (T3DVec3){-2, -2, -2};

private:
    color_t HPBarColor_ = (color_t){0,0,0,0};
};