#include "GO_RepairBoost.h"
#include "../globals.h"

T3DModel* GO_RepairBoost::repairModel = nullptr;
uint8_t GO_RepairBoost::instanceCount = 0;

GO_RepairBoost::GO_RepairBoost(T3DVec3 position) {
    position_ = position;
    lifetime_ = 0;

    takeable_ = true;

    objectWidth_ = 3;

    t3d_mat4_identity(&repairMat);
    repairMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!repairModel) {
        repairModel = t3d_model_load("rom:/repairPoints.t3dm");
    }

    rspq_block_begin();
    t3d_model_draw(repairModel);
    dplRepairBoost = rspq_block_end();
}

GO_RepairBoost::~GO_RepairBoost() {
    free_uncached(repairMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(repairModel);
        repairModel=nullptr;
    }

    rspq_block_free(dplRepairBoost);
}

void GO_RepairBoost::handleInput() {

}

void GO_RepairBoost::update() {
    lifetime_ += global::frameTimeMultiplier;
    t3d_mat4_from_srt_euler(&repairMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, (lifetime_/(5.0f*60.0f))*(2*T3D_PI), 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(repairMatFP, &repairMat);
}

void GO_RepairBoost::renderT3d() {
    t3d_matrix_set(repairMatFP, true);
    //t3d_model_draw(repairModel);
    rspq_block_run(dplRepairBoost);
}

void GO_RepairBoost::renderRdpq() {
    
}

void GO_RepairBoost::consumeEffect() {
    global::audioManager->playSFX("mechaLevelUp3.wav64", {.volume = 0.4f});
    if(global::gameState->theCursor->RPCurrent_ + repairAmount_ > global::gameState->theCursor->RPTotal_) {
        global::gameState->theCursor->RPCurrent_ = global::gameState->theCursor->RPTotal_;
    }
    else {
        global::gameState->theCursor->RPCurrent_ += repairAmount_;
    }
    timeToDelete = true;
}