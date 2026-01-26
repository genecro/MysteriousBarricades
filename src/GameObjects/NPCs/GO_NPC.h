#pragma once

#include <functional>
#include "../GameObject.h"

struct TimedNPCEvent {
    float time;
    std::function<bool()> action; //return true if action can be completed
};

class GO_NPC : public GameObject {
public:
    GO_NPC();
    virtual ~GO_NPC();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;

    void setInteractFunction(std::function<void ()> interactFunction);

    std::function<void ()> interactFunction_;

protected:
    void updateTimeline();
    void updatePositionRotation();
    std::vector<TimedNPCEvent> currTimeline;
    float timelineCtr = 0.0f;

    float intendedRotation_;
    float rotatingSpeed_;
    T3DVec3 intendedPosition_;
    float movingSpeed_;
    bool isMoving_ = false;
};