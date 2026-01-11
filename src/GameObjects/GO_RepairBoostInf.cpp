#include "GO_RepairBoostInf.h"
#include "../globals.h"

T3DModel* GO_RepairBoostInf::repairModel = nullptr;
uint8_t GO_RepairBoostInf::instanceCount = 0;

GO_RepairBoostInf::GO_RepairBoostInf(T3DVec3 position) {
    position_ = position;
    lifetime_ = 0;

    takeable_ = true;

    objectWidth_ = 3;

    t3d_mat4_identity(&repairMat);
    repairMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!repairModel) {
        repairModel = t3d_model_load("rom:/repairPointsInf.t3dm");
    }
}

GO_RepairBoostInf::~GO_RepairBoostInf() {
    free_uncached(repairMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(repairModel);
        repairModel=nullptr;
    }
}

void GO_RepairBoostInf::handleInput() {

}

void GO_RepairBoostInf::update() {
    lifetime_ += global::frameTimeMultiplier;
    t3d_mat4_from_srt_euler(&repairMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, (lifetime_/(5.0f*60.0f))*(2*T3D_PI), 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(repairMatFP, &repairMat);
}

void GO_RepairBoostInf::renderT3d() {
    t3d_matrix_set(repairMatFP, true);
    t3d_model_draw(repairModel);
}

void GO_RepairBoostInf::renderRdpq() {
    
}

void GO_RepairBoostInf::consumeEffect() {
    //global::audioManager->playSFX("mechaLevelUp3.wav64", {.volume = 0.4f});
    if(global::gameState->theCursor->RPCurrent_ < global::gameState->theCursor->RPTotal_) global::gameState->theCursor->RPCurrent_ += global::frameTimeMultiplier;
    else global::gameState->theCursor->RPCurrent_ = global::gameState->theCursor->RPTotal_;
}