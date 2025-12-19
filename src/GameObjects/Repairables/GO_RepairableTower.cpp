#include "GO_RepairableTower.h"

T3DModel* GO_RepairableTower::towerModel = nullptr;
uint8_t GO_RepairableTower::instanceCount = 0;

GO_RepairableTower::GO_RepairableTower(T3DVec3 pos, int HPTotal, int HPCurrent, color_t objColor) {
    position_ = pos;
    HPTotal_ = HPTotal;
    HPCurrent_ = HPCurrent;
    objColor_ = objColor;
    objectWidth_ = 3;

    t3d_mat4_identity(towerMat);
    towerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!towerModel) {
        towerModel = t3d_model_load("rom:/towerComplete.t3dm");
    }
}

GO_RepairableTower::~GO_RepairableTower() {
    free_uncached(towerMatFP);
    instanceCount--;
    if(instanceCount==0) {
        t3d_model_free(towerModel);
        towerModel = nullptr;
    }
}

void GO_RepairableTower::handleInput() {

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
}

void GO_RepairableTower::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(objColor_);
    t3d_matrix_set(towerMatFP, true);
    t3d_model_draw(towerModel);
}