#include "GO_Projectile.h"
#include "../globals.h"

T3DModel* GO_Projectile::projectileModel = nullptr;
uint8_t GO_Projectile::instanceCount = 0;

GO_Projectile::GO_Projectile(T3DVec3 position, float angle, float speed, GameObject* origin, float damage=20) {
    position_ = position;
    lifetime_ = 0;

    angle_ = angle - T3D_PI/2.0f;
    speed_ = speed;

    objectWidth_ = 1;

    damage_ = damage;

    objColor_ = (color_t){0xFF, 0, 0, 0xFF};

    t3d_mat4_identity(&projectileMat);
    projectileMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!projectileModel) {
        projectileModel = t3d_model_load("rom:/projectile.t3dm");
    }

    isProjectile_ = true;
    origin_ = origin;
}

GO_Projectile::~GO_Projectile() {
    free_uncached(projectileMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(projectileModel);
        projectileModel=nullptr;
    }
}

void GO_Projectile::handleInput() {

}

void GO_Projectile::update() {
    lifetime_ += global::frameTimeMultiplier;
    position_ += {speed_*fm_cosf(angle_)*global::frameTimeMultiplier, 0, speed_*fm_sinf(angle_)*global::frameTimeMultiplier};
    t3d_mat4_from_srt_euler(&projectileMat,
        (float[3]){0.005f, 0.005f, 0.005f},
        (float[3]){0.0f, (lifetime_/(5.0f*60.0f))*(2*T3D_PI), 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(projectileMatFP, &projectileMat);
    if(position_.x > global::gameState->envModel->aabbMax[0] || position_.x < global::gameState->envModel->aabbMin[0]
        || position_.z > global::gameState->envModel->aabbMax[2] || position_.z < global::gameState->envModel->aabbMin[2]){
            debugf("Out of bounds\n");
            timeToDelete = true;
    }
}

void GO_Projectile::renderT3d() {
    rdpq_set_prim_color(objColor_);
    t3d_matrix_set(projectileMatFP, true);
    t3d_model_draw(projectileModel);
}

void GO_Projectile::renderRdpq() {
    
}