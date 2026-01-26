#pragma once

#include <functional>
#include "../GameObject.h"
#include "../GO_Projectile.h"
//#include "../../globals.h"

class GO_Repairable: public GameObject {
public:
    GO_Repairable();

    float HPTotal_;
    float HPCurrent_;
    float height_;

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Repairable();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;

    void updateHPBar();
    void drawHPBar();

    void updateRepelRing();
    void renderRepelRing();

    virtual void processProjectile(GO_Projectile* theProjectile);

    float repelEnemyAngleMin_;
    float repelEnemyAngleMax_;

    float HPBarTotalLength_;
    float HPBarCurrentLength_;
    float HPBarHeight_ = 5.0;

    T3DVec3 HPBarPos_ = (T3DVec3){-2, -2, -2};

    bool fullyRepaired = false;

    static T3DModel* repelRingModel;
    static uint8_t rRingInstanceCount;

    virtual GO_Repairable* setRewardFunction(std::function<void()> rewardFunction, bool rewardAlreadyReceived);

    std::function<void()> rewardFunction_;
    bool containsReward_ = false;
    bool rewardAlreadyReceived_ = false;

    GO_Repairable* itself_;

protected:
    sprite_t* rewardIndicatorSprite;
    sprite_t* rewardIndicatorNotFullyRepairedSprite;
    sprite_t* rewardAlreadyReceivedSprite;

    void drawRewardIndicator();

    //rspq_block_t* dplRepairable;

private:
    color_t HPBarColor_ = (color_t){0,0,0,0};

    T3DMat4 repelRingMat;
    T3DMat4FP* repelRingMatFP;
    float repelRingScale;
    float repelRingRotation = 0.0f;

    //rspq_block_t* dplRepelRing;
    
};