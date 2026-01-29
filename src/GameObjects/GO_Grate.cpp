#include "GO_Grate.h"
#include "../globals.h"

T3DModel* GO_Grate::grateModel = nullptr;
uint8_t GO_Grate::instanceCount = 0;

GO_Grate::GO_Grate(T3DVec3 position, float rotation) {
    position_ = position;

    objectWidth_ = 3;
    grateHeight_ = 6.0f;
    

    t3d_mat4_identity(&grateMat);
    grateMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!grateModel) {
        grateModel = t3d_model_load("rom:/grate.t3dm");
    }
    rotation_ = rotation;
    
    
}

GO_Grate::~GO_Grate() {
    free_uncached(grateMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(grateModel);
        grateModel=nullptr;
    }

    //rspq_block_free(dplgrateBoost);
}

void GO_Grate::handleInput() {

}

void GO_Grate::update() {
    if(position_.y < targetY_) {
        position_.y += 0.1f * global::frameTimeMultiplier;
        if(position_.y > targetY_) {
            position_.y = targetY_;
        }
    }

    t3d_mat4_from_srt_euler(&grateMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(grateMatFP, &grateMat);
}

void GO_Grate::renderT3d() {
    if(!updateHasBeenCalled_) return;
    t3d_matrix_set(grateMatFP, true);
    t3d_model_draw(grateModel);
}

void GO_Grate::renderRdpq() {
    
}

void GO_Grate::activate() {
    targetY_ = position_.y + grateHeight_;
}