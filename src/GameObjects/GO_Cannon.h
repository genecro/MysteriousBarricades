#pragma once

#include "GameObject.h"
#include <functional>
#include <vector>

struct TimedCannonEvent {
    float time;
    std::function<bool()> action; //return true if action can be completed
};

class GO_Cannon : public GameObject {
public:
    GO_Cannon(T3DVec3 position, float rotation, float playerYLow, float playerYHigh, color_t cannonColor, color_t projectileColor);
    virtual ~GO_Cannon();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;
    virtual void activate() override;
    bool active_ = true;
    void shootProjectile();


private:
    T3DMat4 cannonMat;
    T3DMat4FP* cannonMatFP;
    static T3DModel* cannonModel;

    static uint8_t instanceCount;

    std::vector<TimedCannonEvent> currTimeline;
    std::vector<TimedCannonEvent> shootTimeline;
    float timelineCtr = 0;

    void updateTimeline();

    

    float playerYHigh_;
    float playerYLow_;
    

    color_t cannonColor_ = (color_t){0xBB, 0xBB, 0xBB, 0xFF};
    color_t projectileColor_;
};