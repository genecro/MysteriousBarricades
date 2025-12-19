#include "GO_Enemy.h"
#include "../../globals.h"

GO_Enemy::GO_Enemy() {

}

GO_Enemy::~GO_Enemy() {
    
}

void GO_Enemy::updateHPBar() {

    HPBarTotalLength_ = HPTotal_*40.0f/100.0f;

    float hpRatio = (float)HPCurrent_/(float)HPTotal_;

    HPBarCurrentLength_ = HPBarTotalLength_*hpRatio-1.0f;
    
    if(hpRatio <= 0.25) HPBarColor_ = (color_t){0xFF, 0, 0, 0xFF};
    else if(hpRatio <= 0.5) HPBarColor_ = (color_t){0xFF, 0x7F, 0, 0xFF};
    else if(hpRatio <= 0.75) HPBarColor_ = (color_t){0xFF, 0xFF, 0, 0xFF};
    else if(hpRatio < 1) HPBarColor_ = (color_t){0, 0xFF, 0, 0xFF};
    else HPBarColor_ = (color_t){0, 0, 0xFF, 0xFF};

    t3d_viewport_calc_viewspace_pos(global::gameState->viewport, HPBarPos_, position_);
}

void GO_Enemy::drawHPBar() {
    rdpq_sync_pipe();
    rdpq_set_mode_fill(RGBA32(0x00, 0x00, 0x00, 0xFF));
    rdpq_fill_rectangle(HPBarPos_.v[0], HPBarPos_.v[1], HPBarPos_.v[0]+HPBarTotalLength_, HPBarPos_.v[1]+HPBarHeight_);

    rdpq_set_mode_fill(HPBarColor_);
    rdpq_fill_rectangle(HPBarPos_.v[0]+1, HPBarPos_.v[1]+1, HPBarPos_.v[0]+HPBarCurrentLength_, HPBarPos_.v[1]+HPBarHeight_-1);
}

void GO_Enemy::setGroundCoord(float newGroundCoord) {
    position_.y = newGroundCoord;
}

void GO_Enemy::pushAwayFromBarricade(T3DVec3 sourcePos, float angle, float distance) {
    if(HPCurrent_ > 0) {
        global::audioManager->playSFX("coinLaser2.wav64", {.volume = 0.8f});
        //barricades 180 degrees apart look the same but push in opposite directions
        //find new positions in both directions and keep the one that's farther away from the barricade
        T3DVec3 newPosition1 = position_+(T3DVec3){distance*fm_sinf(angle), 0, -distance*fm_cosf(angle)};
        T3DVec3 newPosition2 = position_+(T3DVec3){distance*fm_sinf(angle-T3D_PI), 0, -distance*fm_cosf(angle-T3D_PI)};
        
        if(t3d_vec3_distance2(sourcePos, newPosition1) > t3d_vec3_distance2(sourcePos, newPosition2)) {
            position_ = newPosition1;
        }
        else {
            position_ = newPosition2;
        }
    }
}

void GO_Enemy::receiveDamage(float damageAmount) {
    HPCurrent_ -= damageAmount;
}

int GO_Enemy::getState() {
    return enemyState_;
}

void GO_Enemy::setStateAttacking(GO_Repairable* target) {
    target_ = target;
    lifetime_ = 0;
    enemyState_ = global::ENEMY_STATE_ATTACKING;
    rotation_ = fm_atan2f(target_->position_.z - position_.z, target_->position_.x - position_.x);
}

void GO_Enemy::setStateSeeking(GO_Repairable* target) {
    target_ = target;
    enemyState_ = global::ENEMY_STATE_SEEKING;
}