#pragma once

#include "../Enemies/GO_Enemy.h"
#include <functional>
//#include "../globals.h"

struct TimedBossEvent {
    float time;
    std::function<bool()> action; //return true if action can be completed
};

class GO_Boss1: public GO_Enemy {
public:
    GO_Boss1(T3DVec3 pos, float rotation);

    T3DModel* bossModel;

    virtual ~GO_Boss1();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    float intendedRotation_;
    float rotationIncrement_;

    float speedMultiplier = 1.0f;

    uint8_t numHits = 0;

    virtual void processProjectile(GO_Projectile* theProjectile);
    

private:
    T3DMat4 bossMat;
    T3DMat4FP *bossMatFP;

    std::vector<TimedBossEvent> currTimeline;
    std::vector<TimedBossEvent> spinAndShootTimeline;
    float timelineCtr = 0;
    float timelineIdx = 0;

    void updateTimeline();

    void shootProjectile();
};