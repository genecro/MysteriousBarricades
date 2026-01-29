#include "GO_Explosion.h"
#include "../globals.h"

T3DModel* GO_Explosion::explosionModel = nullptr;
uint8_t GO_Explosion::instanceCount = 0;

GO_Explosion::GO_Explosion(T3DVec3 position, float size, color_t color, float lifespan) {
    position_ = position;
    explosionColor_ = color;
    lifespan_ = lifespan;

    t3d_mat4_identity(&explosionMat);
    explosionMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!explosionModel) {
        explosionModel = t3d_model_load("rom:/explosion.t3dm");
    }

    /*
    rspq_block_begin();
    t3d_model_draw(explosionModel);
    dplExplosion = rspq_block_end();
    */
}

GO_Explosion::~GO_Explosion() {
    free_uncached(explosionMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(explosionModel);
        explosionModel=nullptr;
    }

    //rspq_block_free(dplExplosion);
}

void GO_Explosion::handleInput() {

}

void GO_Explosion::update() {
    if(!updateHasBeenCalled_) {
        updateHasBeenCalled_ = true;
    }
    lifetime_ += global::frameTimeMultiplier;
    if(lifetime_ >= lifespan_) timeToDelete = true;

    explosionColor_.a = fmax(255.0f*(lifespan_-lifetime_) / lifespan_, 0.0f);
    //if(explosionColor_.a<0) explosionColor_.a = 0;

    float tempSize = 0.02f + 0.04f*(lifetime_/lifespan_);

    t3d_mat4_from_srt_euler(&explosionMat,
        (float[3]){tempSize, tempSize, tempSize},
        (float[3]){0.0f, (lifetime_/lifespan_)*(4*T3D_PI), 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(explosionMatFP, &explosionMat);
}

void GO_Explosion::renderRdpq() {

}

void GO_Explosion::renderT3d() {
    if(!updateHasBeenCalled_) return;
    rdpq_sync_pipe();

    rdpq_set_prim_color(explosionColor_);
    t3d_matrix_set(explosionMatFP, true);
    t3d_model_draw(explosionModel);
    //rspq_block_run(dplExplosion);
    
}