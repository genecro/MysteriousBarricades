#include "GO_EnemyRushToBarricade.h"
#include "../../globals.h"
#include "../GO_Explosion.h"
#include "../GO_RepairBoost.h"

T3DModel* GO_EnemyRushToBarricade::enemyModel = nullptr;
uint8_t GO_EnemyRushToBarricade::instanceCount = 0;

GO_EnemyRushToBarricade::GO_EnemyRushToBarricade(T3DVec3 pos, GO_Repairable* target, bool dropItem = true) {
    //debugf("Entering second constructor\n");
    position_ = pos;
    HPTotal_ = 100;
    HPCurrent_ = 100;
    objColor_ = color_t{0xFF, 0, 0xFF, 0xFF};
    target_ = target;

    attackRate = 3*60;
    attackDamage = 1;

    objectWidth_ = 2;
    attackAnimDistance = objectWidth_;

    isStunned_ = false;
    isInvincible_ = false;

    dropItem_ = dropItem;

    enemyState_ = global::ENEMY_STATE_SEEKING;

    if(target_) {
        targetPos_ = target_->position_;
    }

    //debugf("targetPos_.x = %.2f\n", targetPos_.x);
    //debugf("targetPos_.z = %.2f\n\n", targetPos_.z);

    //rotation_ = fm_atan2f(target_->position_.z - position_.z, target_->position_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
    //rotation_ = fm_atan2f(targetPos_.z - position_.z, targetPos_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
    rotation_ = (float)rand()/(float)RAND_MAX * 2 * T3D_PI;
    intendedRotation_ = rotation_;
    rotationIncrement_ = T3D_PI / 128.0f;

    speed_ = baseSpeed_;

    t3d_mat4_identity(enemyMat);
    enemyMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!enemyModel) {
        enemyModel = t3d_model_load("rom:/enemyRushToBarricade.t3dm");
    }
}

GO_EnemyRushToBarricade::GO_EnemyRushToBarricade(T3DVec3 pos, T3DVec3 targetPos, bool dropItem = true) : GO_EnemyRushToBarricade(pos, nullptr, dropItem) {
    //debugf("Entering first constructor\n");
    targetPos_ = targetPos;
    // debugf("targetPos_.x = %.2f\n", targetPos_.x);
    // debugf("targetPos_.z = %.2f\n\n", targetPos_.z);
    //GO_EnemyRushToBarricade(pos, nullptr, dropItem);
}

GO_EnemyRushToBarricade::~GO_EnemyRushToBarricade() {
    free_uncached(enemyMatFP);
    instanceCount--;
    if(instanceCount==0) {
        t3d_model_free(enemyModel);
        enemyModel = nullptr;
    }
}

void GO_EnemyRushToBarricade::handleInput() {

}

void GO_EnemyRushToBarricade::update() {
    float prevLifetime = lifetime_;
    lifetime_ += global::frameTimeMultiplier;

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

    else {
        switch(enemyState_) {
            case global::ENEMY_STATE_SEEKING: {
                //not stunned, moves normally
                if(isMoving_ && !isStunned_) {
                    //rotate randomly towards the target every 5 seconds
                    if((int)(prevLifetime / (60.0f*5.0f)) != (int)(lifetime_ / (60.0f*5.0f))) {
                        //rotation_ = fm_atan2f(target_->position_.z - position_.z, target_->position_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
                        //rotation_ = fm_atan2f(targetPos_.z - position_.z, targetPos_.x- position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
                        intendedRotation_ = fm_atan2f(targetPos_.z - position_.z, targetPos_.x- position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
                    }

                    if(abs(rotation_-intendedRotation_) <= rotationIncrement_) rotation_ = intendedRotation_;
                    else if(std::remainder(rotation_ - intendedRotation_, T3D_PI*2.0f) > 0) {
                        rotation_ -= rotationIncrement_ * global::frameTimeMultiplier;
                        if(rotation_ < 0) rotation_ += T3D_PI*2.0f;
                    }
                    else {
                        rotation_ += rotationIncrement_ * global::frameTimeMultiplier;
                        if(rotation_ <= T3D_PI*2.0f) rotation_ -= T3D_PI*2.0f;
                    }

                    //move forward
                    position_.x += speed_*fm_cosf(rotation_)*global::frameTimeMultiplier;
                    position_.z += speed_*fm_sinf(rotation_)*global::frameTimeMultiplier;
                }
                
            } break;



            case global::ENEMY_STATE_ATTACKING: {
                if((int)(prevLifetime / attackRate) != (int)(lifetime_ / attackRate)) {
                    attackTarget();
                    global::audioManager->playSFX("gutKick6.wav64", {.volume = 0.4f});
                }
                float atkWindup = fmod(lifetime_, attackRate);
                attackAnimOffset = (T3DVec3){attackAnimDistance*atkWindup/attackRate*fm_cosf(rotation_), 0.0f, attackAnimDistance*atkWindup/attackRate*fm_sinf(rotation_)};
            } break;

            case global::ENEMY_STATE_CHASING_CURSOR: {
                intendedRotation_ = fm_atan2f(targetPos_.z - position_.z, targetPos_.x- position_.x);
                
                if(abs(rotation_-intendedRotation_) <= rotationIncrement_ * speed_ / baseSpeed_ * global::frameTimeMultiplier) rotation_ = intendedRotation_;
                else if(std::remainder(rotation_ - intendedRotation_, T3D_PI*2.0f) > 0) {
                    rotation_ -= rotationIncrement_ * speed_ / baseSpeed_ * global::frameTimeMultiplier;
                    if(rotation_ < 0) rotation_ += T3D_PI*2.0f;
                }
                else {
                    rotation_ += rotationIncrement_ * speed_ / baseSpeed_ * global::frameTimeMultiplier;
                    if(rotation_ <= T3D_PI*2.0f) rotation_ -= T3D_PI*2.0f;
                }

                //move forward
                position_.x += speed_*fm_cosf(rotation_)*global::frameTimeMultiplier;
                position_.z += speed_*fm_sinf(rotation_)*global::frameTimeMultiplier;
            } break;
        }
    }

    

    t3d_mat4_from_srt_euler(&enemyMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, rotation_, 0.0f},
        (position_ - attackAnimOffset).v
    );
    t3d_mat4_to_fixed(enemyMatFP, &enemyMat);

    updateHPBar();
    if(HPCurrent_<=0) {
        timeToDelete = true;
        global::gameState->enemyDestroyed();
        global::audioManager->playSFX("cruncher5.wav64", {.volume = 0.4f});
        if(dropItem_) global::gameState->objectList->push(new GO_RepairBoost(position_));
        global::gameState->objectList->push(new GO_Explosion(position_, 5, (color_t){0xFF, 0x77, 0x00, 0xFF}, 2*60));
    }
}

void GO_EnemyRushToBarricade::renderRdpq() {
    drawHPBar();
}

void GO_EnemyRushToBarricade::renderT3d() {
    if(displayModel_) {
        rdpq_sync_pipe();

        rdpq_set_prim_color(objColor_);
        t3d_matrix_set(enemyMatFP, true);
        t3d_model_draw(enemyModel);
    }
}

void GO_EnemyRushToBarricade::stun(float stunTimeSeconds) {
    isStunned_ = true;
    isInvincible_ = true;
    stunCooldown_ = stunTimeSeconds*60.f;
}

void GO_EnemyRushToBarricade::attackTarget() {
    if(target_) target_->HPCurrent_ -= attackDamage;
}

void GO_EnemyRushToBarricade::cursorMakingBarricade(T3DVec3 cursorPos) {
    targetPos_ = cursorPos;
    enemyState_ = global::ENEMY_STATE_CHASING_CURSOR;
    speed_ = chasingCursorSpeed_;
}

void GO_EnemyRushToBarricade::cursorNotMakingBarricade() {
    targetPos_ = target_->position_;
    enemyState_ = global::ENEMY_STATE_SEEKING;
    speed_ = baseSpeed_;
}