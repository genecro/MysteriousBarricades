#include "GO_EnemyBasic.h"
#include "../../globals.h"

T3DModel* GO_EnemyBasic::enemyModel = nullptr;
uint8_t GO_EnemyBasic::instanceCount = 0;

GO_EnemyBasic::GO_EnemyBasic(T3DVec3 pos, T3DVec3 target) {
    position_ = pos;
    HPTotal_ = 100;
    HPCurrent_ = 100;
    objColor_ = color_t{0, 0xFF, 0, 0xFF};
    target_ = target;

    rotation_ = fm_atan2f(target_.z - position_.z, target_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));

    speed_ = 0.02;

    t3d_mat4_identity(enemyMat);
    enemyMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!enemyModel) {
        enemyModel = t3d_model_load("rom:/enemyBasic.t3dm");
    }
}

GO_EnemyBasic::~GO_EnemyBasic() {
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

    if((int)(prevLifetime / (60.0f*5.0f)) != (int)(lifetime_ / (60.0f*5.0f))) {
        rotation_ = fm_atan2f(target_.z - position_.z, target_.x - position_.x) + (((float)rand() / (float)RAND_MAX)*(T3D_PI / 2.0f) - (T3D_PI / 4.0f));
    }

    position_.x += speed_*fm_cosf(rotation_)*global::frameTimeMultiplier;
    position_.z += speed_*fm_sinf(rotation_)*global::frameTimeMultiplier;

    t3d_mat4_from_srt_euler(&enemyMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(enemyMatFP, &enemyMat);

    updateHPBar();
}

void GO_EnemyBasic::renderRdpq() {
    drawHPBar();
}

void GO_EnemyBasic::renderT3d() {
    rdpq_sync_pipe();

    rdpq_set_prim_color(objColor_);
    t3d_matrix_set(enemyMatFP, true);
    t3d_model_draw(enemyModel);
}

