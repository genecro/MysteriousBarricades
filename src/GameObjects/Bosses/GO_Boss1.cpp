#include "GO_Boss1.h"
#include "../../globals.h"
#include "../GO_Explosion.h"
#include "../GO_RepairBoostInf.h"
#include <math.h>

GO_Boss1::GO_Boss1(T3DVec3 pos, float rotation) {
    position_ = pos;
    rotation_ = rotation;
    intendedRotation_ = rotation_;
    rotationIncrement_ = T3D_PI/128.0f;

    HPTotal_ = 200;
    HPCurrent_ = 200;

    objectWidth_ = 3.0f;
    

    t3d_mat4_identity(bossMat);
    bossMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    bossModel = t3d_model_load("rom:/boss1.t3dm");

    spinAndShootTimeline.push_back({
        10,
        [&](){
            //dummy command for when currTimeline gets set by a TimedBossAction
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            shootProjectile();
            return true;
        }
    });
    /*
    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            intendedRotation_ = std::remainder(intendedRotation_ + T3D_PI/2.0f, 2.0f*T3D_PI);
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            shootProjectile();
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            intendedRotation_ = std::remainder(intendedRotation_ + T3D_PI/2.0f, 2.0f*T3D_PI);
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            shootProjectile();
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            intendedRotation_ = std::remainder(intendedRotation_ + T3D_PI/2.0f, 2.0f*T3D_PI);
            return true;
        }
    });

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            shootProjectile();
            return true;
        }
    });
    */

    spinAndShootTimeline.push_back({
        4*60,
        [&](){
            intendedRotation_ = std::remainder(intendedRotation_ + T3D_PI/2.0f, 2.0f*T3D_PI);
            currTimeline = spinAndShootTimeline;
            return true;
        }
    });

    currTimeline = spinAndShootTimeline;
}

GO_Boss1::~GO_Boss1() {
    free_uncached(bossMatFP);
    t3d_model_free(bossModel);
}

void GO_Boss1::handleInput() {

}

void GO_Boss1::update() {
    if(!isStunned_){
        //debugf("Not stunned\n");
        updateTimeline();
        //debugf("Timeline updated\n");

        if(abs(rotation_-intendedRotation_) <= rotationIncrement_ * 3.0f) rotation_ = intendedRotation_;
        else if(std::remainder(rotation_ - intendedRotation_, T3D_PI*2.0f) > 0) {
            rotation_ = std::remainder(rotation_ - rotationIncrement_ * global::frameTimeMultiplier * speedMultiplier, T3D_PI*2.0f);
        }
        else {
            rotation_ = std::remainder(rotation_ + rotationIncrement_ * global::frameTimeMultiplier * speedMultiplier, T3D_PI*2.0f);
        }

        //debugf("rotated\n");
    }
    if(isStunned_) {
        //stunned, model flashes and can't move or be damaged until cooldown reaches 0
        float prevStunCooldown = stunCooldown_;
        stunCooldown_ -= global::frameTimeMultiplier;
        if(stunCooldown_ <= 0) {
            isStunned_ = false;
            isInvincible_ = false;
            stunCooldown_ = 0;
            displayModel_ = true;
        }
        else if((int)(prevStunCooldown / (5.0f)) != (int)(stunCooldown_ / (5.0f))) {
            displayModel_ = !displayModel_;
        }     
    }
    t3d_mat4_from_srt_euler(&bossMat,
        (float[3]){0.08f, 0.08f, 0.08f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(bossMatFP, &bossMat);

    updateHPBar();

    if(HPCurrent_ <= 0) {
        global::audioManager->playSFX("bossDying.wav64", {.volume = 0.4f});
        global::gameState->objectList->push(new GO_RepairBoostInf((T3DVec3){0,position_.y,0}));
        global::gameState->objectList->push(new GO_Explosion((T3DVec3){2,position_.y,0}, 10.0f, (color_t){0xFF, 0x77, 0x00, 0xFF}, 4*60));
        global::gameState->objectList->push(new GO_Explosion((T3DVec3){-2,position_.y,2}, 10.0f, (color_t){0xFF, 0x77, 0x00, 0xFF}, 4*60));
        global::gameState->objectList->push(new GO_Explosion((T3DVec3){-2,position_.y,-2}, 10.0f, (color_t){0xFF, 0x77, 0x00, 0xFF}, 4*60));
        timeToDelete = true;
    }
}

void GO_Boss1::renderRdpq() {
    //draw HP bar
    drawHPBar();
}

void GO_Boss1::renderT3d() {
    if(displayModel_) {
        t3d_matrix_set(bossMatFP, true);
        t3d_model_draw(bossModel);
    }
}

void GO_Boss1::updateTimeline() {
    if(!isStunned_) timelineCtr += global::frameTimeMultiplier * speedMultiplier;
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

void GO_Boss1::processProjectile(GO_Projectile* theProjectile) {
    if((theProjectile->origin_ != this || theProjectile->reflected) &&
            abs(theProjectile->position_.x - position_.x) < theProjectile->objectWidth_ + objectWidth_ &&
            abs(theProjectile->position_.z - position_.z) < theProjectile->objectWidth_ + objectWidth_) {
        HPCurrent_ -= theProjectile->damage_;
        global::audioManager->playSFX("mechaDamage5.wav64", {.volume = 0.4f});
        //stun? maybe stun after 4 hits?
        theProjectile->timeToDelete = true;
        numHits++;
        if(numHits % 3 == 0) {
            stun(4.0f);
            speedMultiplier+=0.6f;
        }
    }
}

void GO_Boss1::shootProjectile() {
    global::gameState->objectList->push(new GO_Projectile((T3DVec3){3.0f*cosf(rotation_+T3D_PI/2.0f), 5.0f, 3.0f*sinf(rotation_+T3D_PI/2.0f)}, rotation_+T3D_PI, 0.3f * speedMultiplier, this));
    global::audioManager->playSFX("gearedPunch5.wav64", {.volume = 0.4f});
}