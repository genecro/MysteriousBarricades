#include "GO_RepairableTower.h"
#include "../../globals.h"

T3DModel* GO_RepairableTower::towerModel = nullptr;
uint8_t GO_RepairableTower::instanceCount = 0;

GO_RepairableTower::GO_RepairableTower(T3DVec3 pos, int HPTotal, int HPCurrent, color_t objColor, float repelAngleMin, float repelAngleMax) {
    position_ = pos;
    HPTotal_ = HPTotal;
    HPCurrent_ = HPCurrent;
    objColor_ = objColor;
    objectWidth_ = 2;
    repelEnemyAngleMin_ = repelAngleMin;
    repelEnemyAngleMax_ = repelAngleMax;
    height_ = 7;

    t3d_mat4_identity(towerMat);
    towerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!towerModel) {
        towerModel = t3d_model_load("rom:/towerComplete.t3dm");
    }
    /*
    rspq_block_begin();
    t3d_model_draw(towerModel);
    dplRepairable = rspq_block_end();
    */
    rewardIndicatorSprite = sprite_load("rom:/sprites/rewardIndicator.rgba32.sprite");
    rewardIndicatorNotFullyRepairedSprite = sprite_load("rom:/sprites/rewardIndicatorNotFullyRepaired.rgba32.sprite");
    rewardAlreadyReceivedSprite = sprite_load("rom:/sprites/rewardAlreadyReceived.rgba32.sprite");

    itself_ = this;
}

GO_RepairableTower::~GO_RepairableTower() {
    free_uncached(towerMatFP);
    instanceCount--;
    if(instanceCount==0) {
        t3d_model_free(towerModel);
        towerModel = nullptr;
    }
    sprite_free(rewardIndicatorSprite);
    sprite_free(rewardIndicatorNotFullyRepairedSprite);
    sprite_free(rewardAlreadyReceivedSprite);

    //rspq_block_free(dplRepairable);
}

void GO_RepairableTower::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(btn.a) {
        if(containsReward_ && fullyRepaired &&
            abs(global::gameState->theCursor->position_.x - position_.x) <= global::gameState->theCursor->objectWidth_ + objectWidth_ &&
            abs(global::gameState->theCursor->position_.z - position_.z) <= global::gameState->theCursor->objectWidth_ + objectWidth_) {
                //push current state onto stack, load
                rewardFunction_();
                global::gameState->theCursor->setStateBaseState();
        }
    }
}

void GO_RepairableTower::update() {
    t3d_mat4_from_srt_euler(&towerMat,
        (float[3]){0.03f, 0.03f, 0.03f},
        (float[3]){0.0f, 0.0f, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(towerMatFP, &towerMat);

    updateHPBar();
}

void GO_RepairableTower::renderRdpq() {
    drawHPBar();
    drawRewardIndicator();
}

void GO_RepairableTower::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(objColor_);
    t3d_matrix_set(towerMatFP, true);
    t3d_model_draw(towerModel);

    //rspq_block_run(dplRepairable);
}

void GO_RepairableTower::processProjectile(GO_Projectile* theProjectile) {
    if(abs(theProjectile->position_.x - position_.x) < theProjectile->objectWidth_ + objectWidth_ &&
        abs(theProjectile->position_.z - position_.z) < theProjectile->objectWidth_ + objectWidth_) {

        if(!fullyRepaired) {
            HPCurrent_ -= theProjectile->damage_;
            global::audioManager->playSFX("thud5.wav64", {.volume = 0.4f});
            theProjectile->timeToDelete = true;
        }
        else {
            global::audioManager->playSFX("coinImpact2.wav64", {.volume = 0.4f});
            theProjectile->timeToDelete = true;
        }
    }
}