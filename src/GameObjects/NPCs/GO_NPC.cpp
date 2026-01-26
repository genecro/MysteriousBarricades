#include "GO_NPC.h"
#include "../../globals.h"

GO_NPC::GO_NPC() {

}

GO_NPC::~GO_NPC() {

}

void GO_NPC::setInteractFunction(std::function<void ()> interactFunction) {
    interactFunction_ = interactFunction;
}

void GO_NPC::updateTimeline() {
    timelineCtr += global::frameTimeMultiplier;
    if(!currTimeline.empty() && timelineCtr >= currTimeline.front().time) {
        if(currTimeline.front().action()) {
            currTimeline.erase(currTimeline.begin());
        }
        else {
            currTimeline.front().time = 5*60; // try again in 5 seconds
        }
        timelineCtr = 0;
    }
}

void GO_NPC::updatePositionRotation() {
    //simple rotation towards intended rotation
    float rotationDiff = std::remainder(intendedRotation_ - rotation_, 2.0f*T3D_PI);
    if(fabs(rotationDiff) < T3D_PI/180.0f) {
        rotation_ = intendedRotation_;
    }
    else if(rotationDiff > 0) {
        rotation_ += rotatingSpeed_ * global::frameTimeMultiplier;
    }
    else {
        rotation_ -= rotatingSpeed_ * global::frameTimeMultiplier;
    }

    //simple movement towards intended position
    T3DVec3 posDiff = intendedPosition_ - position_;
    float distance = t3d_vec3_len(posDiff);
    if(distance < 0.1f) {
        position_ = intendedPosition_;
        isMoving_ = false;
    }
    else {
        t3d_vec3_norm(posDiff);
        position_ += posDiff * movingSpeed_ * global::frameTimeMultiplier;
        isMoving_ = true;
    }
}