#include "GO_Cannon.h"
#include "../globals.h"

T3DModel* GO_Cannon::cannonModel = nullptr;
uint8_t GO_Cannon::instanceCount = 0;

GO_Cannon::GO_Cannon(T3DVec3 position, float rotation, float playerYLow, float playerYHigh) {
    position_ = position;

    objectWidth_ = 3;
    
    playerYLow_ = playerYLow;
    playerYHigh_ = playerYHigh;
    
    t3d_mat4_identity(&cannonMat);
    cannonMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!cannonModel) {
        cannonModel = t3d_model_load("rom:/cannon.t3dm");
    }
    rotation_ = rotation;

    shootTimeline.push_back({
        10,
        [&](){
            //dummy command for when currTimeline gets set by a TimedBossAction
            return true;
        }
    });

    shootTimeline.push_back({
        4*60,
        [&](){
            shootProjectile();
            currTimeline = shootTimeline;
            return true;
        }
    });
    
    currTimeline = shootTimeline;
}

GO_Cannon::~GO_Cannon() {
    free_uncached(cannonMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(cannonModel);
        cannonModel=nullptr;
    }

    //rspq_block_free(dplcannonBoost);
}

void GO_Cannon::handleInput() {

}

void GO_Cannon::update() {
    if(active_ && global::gameState->thePlayer_->position_.z >= playerYLow_ && global::gameState->thePlayer_->position_.z <= playerYHigh_) {
        updateTimeline();
    }

    t3d_mat4_from_srt_euler(&cannonMat,
        (float[3]){0.06f, 0.06f, 0.06f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(cannonMatFP, &cannonMat);
}

void GO_Cannon::renderT3d() {
    //if(!updateHasBeenCalled_) return;
    t3d_matrix_set(cannonMatFP, true);
    t3d_model_draw(cannonModel);
}

void GO_Cannon::renderRdpq() {
    
}


void GO_Cannon::updateTimeline() {
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

void GO_Cannon::shootProjectile() {
    global::audioManager->playSFX("gearedPunch5.wav64", {.volume = 0.4f});
    global::gameState->objectList->push(new GO_Projectile(
        position_,
        rotation_ + T3D_PI,
        0.3f,
        this,
        1.0f
    ));
}

void GO_Cannon::activate() {
    active_ = false;
}