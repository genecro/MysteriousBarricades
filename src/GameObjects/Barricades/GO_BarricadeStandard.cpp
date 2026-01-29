#include "GO_BarricadeStandard.h"
#include "../../globals.h"

T3DModel* GO_BarricadeStandard::barricadeModel = nullptr;
uint8_t GO_BarricadeStandard::instanceCount = 0;

GO_BarricadeStandard::GO_BarricadeStandard(T3DVec3 pos, T3DVec3 sizeRotation, color_t objColor) {
    position_ = pos;
    rotation_ = fm_atan2f(sizeRotation.z, sizeRotation.x);
    scaleFactor_ = 0.015;
    scale_ = sqrt(pow(sizeRotation.x, 2) + pow(sizeRotation.z, 2))*scaleFactor_;
    objColor_ = objColor;

    failedTimerMax_ = 1*60;

    t3d_mat4_identity(barricadeMat);
    barricadeMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!barricadeModel) {
        barricadeModel = t3d_model_load("rom:/barricadeStandard.t3dm");
    }
    /*
    rspq_block_begin();
    t3d_model_draw(barricadeModel);
    dplBarricade = rspq_block_end();
    */
    debugf("Barricade scale = %.2f\n", scale_);
}

GO_BarricadeStandard::~GO_BarricadeStandard() {
    free_uncached(barricadeMatFP);
    instanceCount--;
    if(instanceCount==0) {
        t3d_model_free(barricadeModel);
        barricadeModel = nullptr;
    }
}

void GO_BarricadeStandard::handleInput() {

}

void GO_BarricadeStandard::update() {

    if(castHasFailed_) {
        failedTimer_-=global::frameTimeMultiplier;
        objColor_ = (color_t){0x77, 0x77, 0x77, fmax(255.0f*failedTimer_ / failedTimerMax_, 0.0f)};
        if(failedTimer_ <= 0) timeToDelete = true;
    }

    t3d_mat4_from_srt_euler(&barricadeMat,
        (float[3]){scale_, 0.04f, 1.0f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(barricadeMatFP, &barricadeMat);

    //castSuccess_ = true;
    checkTimeLeft();
}

void GO_BarricadeStandard::renderRdpq() {

}

void GO_BarricadeStandard::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(objColor_);
    t3d_matrix_set(barricadeMatFP, true);
    t3d_model_draw(barricadeModel);
    //rspq_block_run(dplBarricade);
}

//TODO: move this to GO_Barricade and create affectEnemy function here
void GO_BarricadeStandard::processEnemy(GO_Enemy* theEnemy) {
    if(checkCollision(theEnemy)) {
        
        if(castSuccess_ && !castHasFailed_ && !theEnemy->isInvincible_) {
            //theEnemy->HPCurrent_ -= theEnemy->HPTotal_/2.0f;
            theEnemy->receiveDamage(theEnemy->HPTotal_/2);
            theEnemy->stun(2);
            theEnemy->pushAwayFromBarricade(position_, rotation_, 5);
            timeToDelete = true;
        }

        else if(!castSuccess_ && !castHasFailed_) {
            castHasFailed_ = true;
            failedTimer_ = failedTimerMax_;
            global::gameState->barricadeCastFailed();
            global::audioManager->playSFX("metallicDodgeChance5.wav64", {.volume = 0.4f});
        }
    }
}

void GO_BarricadeStandard::castSuccess() {
    castSuccess_ = true;
    global::gameState->barricadeCreated();
    global::audioManager->playSFX("bitSweep1.wav64", {.volume = 0.4f});
    debugf("Barricade length: %.2f\n", scale_/scaleFactor_);
}