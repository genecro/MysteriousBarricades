#include "GO_EnemyBasic.h"
#include "../../globals.h"
#include "../GO_Explosion.h"
#include "../GO_RepairBoost.h"

T3DModel* GO_EnemyBasic::enemyModel = nullptr;
uint8_t GO_EnemyBasic::instanceCount = 0;

GO_EnemyBasic::GO_EnemyBasic(T3DVec3 pos, GO_Repairable* target) {
    position_ = pos;
    HPTotal_ = 100;
    HPCurrent_ = 100;
    objColor_ = color_t{0, 0xFF, 0, 0xFF};
    target_ = target;

    attackRate = 3*60;
    attackDamage = 1;

    objectWidth_ = 2;
    attackAnimDistance = objectWidth_;

    isStunned_ = false;
    isInvincible_ = false;

    rotation_ = fm_atan2f(target_->position_.z - position_.z, target_->position_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));

    speed_ = 0.02;

    t3d_mat4_identity(enemyMat);
    enemyMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!enemyModel) {
        enemyModel = t3d_model_load("rom:/enemyBasic.t3dm");
    }
}

GO_EnemyBasic::~GO_EnemyBasic() {
    global::gameState->objectList->push(new GO_RepairBoost(position_));
    global::gameState->objectList->push(new GO_Explosion(position_, 5, (color_t){0xFF, 0x77, 0x00, 0xFF}, 2*60));
    
    
    free_uncached(enemyMatFP);
    instanceCount--;
    if(instanceCount==0) {
        t3d_model_free(enemyModel);
        enemyModel = nullptr;
    }
}

void GO_EnemyBasic::handleInput() {

}

void GO_EnemyBasic::update() {
    float prevLifetime = lifetime_;
    lifetime_ += global::frameTimeMultiplier;

    switch(enemyState_) {
        case global::ENEMY_STATE_SEEKING:
            //not stunned, moves normally
            if(!isStunned_) {
                //rotate randomly towards the target every 5 seconds
                if((int)(prevLifetime / (60.0f*5.0f)) != (int)(lifetime_ / (60.0f*5.0f))) {
                    rotation_ = fm_atan2f(target_->position_.z - position_.z, target_->position_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
                }

                //move forward
                position_.x += speed_*fm_cosf(rotation_)*global::frameTimeMultiplier;
                position_.z += speed_*fm_sinf(rotation_)*global::frameTimeMultiplier;
            }
            //stunned, model flashes and can't move or be damaged until cooldown reaches 0
            else {
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
        break;

        case global::ENEMY_STATE_ATTACKING:
            if((int)(prevLifetime / attackRate) != (int)(lifetime_ / attackRate)) {
                attackTarget();
                //play attack sound
                global::audioManager->playSFX("gutKick6.wav64", {.volume = 0.4f});
            }
            float atkWindup = fmod(lifetime_, attackRate);
            attackAnimOffset = (T3DVec3){attackAnimDistance*atkWindup/attackRate*fm_cosf(rotation_), 0.0f, attackAnimDistance*atkWindup/attackRate*fm_sinf(rotation_)};
        break;
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
        global::audioManager->playSFX("cruncher5.wav64", {.volume = 0.4f});
    }
}

void GO_EnemyBasic::renderRdpq() {
    drawHPBar();
}

void GO_EnemyBasic::renderT3d() {
    if(displayModel_) {
        rdpq_sync_pipe();

        rdpq_set_prim_color(objColor_);
        t3d_matrix_set(enemyMatFP, true);
        t3d_model_draw(enemyModel);
    }
}

void GO_EnemyBasic::stun(float stunTimeSeconds) {
    isStunned_ = true;
    isInvincible_ = true;
    stunCooldown_ = stunTimeSeconds*60.f;
}

void GO_EnemyBasic::attackTarget() {
    target_->HPCurrent_ -= attackDamage;
}