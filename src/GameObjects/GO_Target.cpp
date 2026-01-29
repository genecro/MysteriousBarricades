#include "GO_Target.h"
#include "../globals.h"

T3DModel* GO_Target::targetModel = nullptr;
uint8_t GO_Target::instanceCount = 0;

GO_Target::GO_Target(T3DVec3 position, float rotation, std::vector<GameObject*>* objectsToActivate, std::function<void()> onActivate) {
    position_ = position;

    objectWidth_ = 3;
    
    objectsToActivate_ = objectsToActivate;
    onActivate_ = onActivate;

    t3d_mat4_identity(&targetMat);
    targetMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    instanceCount++;
    if(!targetModel) {
        targetModel = t3d_model_load("rom:/target.t3dm");
    }
    rotation_ = rotation;
    
    
}

GO_Target::~GO_Target() {
    free_uncached(targetMatFP);
    instanceCount--;
    if(instanceCount == 0) {
        t3d_model_free(targetModel);
        targetModel=nullptr;
    }
}

void GO_Target::handleInput() {

}

void GO_Target::update() {

    t3d_mat4_from_srt_euler(&targetMat,
        (float[3]){0.02f, 0.02f, 0.02f},
        (float[3]){0.0f, rotation_, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(targetMatFP, &targetMat);
}

void GO_Target::renderT3d() {
    rdpq_sync_pipe();
    rdpq_set_prim_color(targetColor_);
    t3d_matrix_set(targetMatFP, true);
    t3d_model_draw(targetModel);
}

void GO_Target::renderRdpq() {
    
}

bool GO_Target::checkProjectionCollision(GameObject* p) {
    float dx = p->position_.x - position_.x;
    float dz = p->position_.z - position_.z;
    float distanceSquared = dx*dx + dz*dz;
    float combinedRadius = (objectWidth_ + p->objectWidth_) * 0.5f;
    return distanceSquared <= (combinedRadius * combinedRadius);
}

void GO_Target::checkProjectiles() {
    for(auto& obj : *global::gameState->objectList->gameObjects) {
        if(obj->isProjectile_) {
            if(checkProjectionCollision(obj)) {
                //global::audioManager->playSFX("targetHit.wav64", {.volume=0.5f});
                if(objectsToActivate_) {
                    for(auto& obj : *objectsToActivate_) {
                        obj->activate();
                    }
                }
                if(onActivate_) {
                    onActivate_();
                }
            }
        }
    }
}