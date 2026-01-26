#include "GO_NPCKnight.h"
#include "../../globals.h"

#define FB_COUNT 3

T3DModel* GO_NPCKnight::knightModel = nullptr;
uint8_t GO_NPCKnight::instanceCount = 0;

GO_NPCKnight::GO_NPCKnight(std::string name, T3DVec3 position, float rotation) {
    name_ = name;
    objectWidth_ = 4.0f;
    position_ = position;
    rotation_ = rotation;
    intendedRotation_ = rotation_;
    intendedPosition_ = position_;
    movingSpeed_ = 0.05f;
    rotatingSpeed_ = 0.05f;

    

    isSolid_ = true;

    instanceCount++;
    if(!knightModel) {
        knightModel = t3d_model_load("rom:/N64Knight.t3dm");
    }
    knightMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);

    skel = t3d_skeleton_create_buffered(knightModel, FB_COUNT);
    skelBlend = t3d_skeleton_clone(&skel, false);
    animIdle = t3d_anim_create(knightModel, "Idle");
    t3d_anim_attach(&animIdle, &skel);
    animWalk = t3d_anim_create(knightModel, "Walk");
    t3d_anim_attach(&animWalk, &skelBlend);
    frameIdx = 0;
    /*
    rspq_block_begin();
    t3d_model_draw_skinned(knightModel, &skel);
    dplKnight = rspq_block_end();
    */
}

GO_NPCKnight::~GO_NPCKnight() {
    instanceCount--;
    if(instanceCount == 0 && knightModel) {
        t3d_model_free(knightModel);
        knightModel = nullptr;
    }
    t3d_skeleton_destroy(&skel);
    t3d_skeleton_destroy(&skelBlend);

    t3d_anim_destroy(&animIdle);
    t3d_anim_destroy(&animWalk);

    //rspq_block_free(dplKnight);
}

void GO_NPCKnight::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.a) {
        if(global::gameState->thePlayer_->canInteract(position_, objectWidth_)) {
            if(interactFunction_) {
                //face the player
                intendedRotation_ = atan2f(global::gameState->thePlayer_->position_.z - position_.z,
                                            global::gameState->thePlayer_->position_.x - position_.x);
                interactFunction_();
            }  
        }
    }
}

void GO_NPCKnight::update() {
    frameIdx = (frameIdx + 1) % FB_COUNT;
    
    updateTimeline();
    
    updatePositionRotation();
    if(isMoving_) {
        animBlend = 1.0f;
    }
    else {
        animBlend = 0.0f;
    }

    float deltaTime = 1.0f/60.0f * global::frameTimeMultiplier;
    t3d_anim_update(&animIdle, deltaTime);
    t3d_anim_set_speed(&animWalk, animBlend + 0.15f);
    t3d_anim_update(&animWalk, deltaTime);

    t3d_skeleton_blend(&skel, &skel, &skelBlend, animBlend);
    t3d_skeleton_update(&skel);

    t3d_mat4fp_from_srt_euler(&knightMatFP[frameIdx],
        (float[3]){0.07f, 0.07f, 0.07f},
        (float[3]){0.0f, rotation_- T3D_PI / 2.0f, 0.0f},
        (position_/*+(T3DVec3){{0, objectWidth_/2.0f, 0}}*/).v
    );
    checkCollision();
}

void GO_NPCKnight::renderT3d() {
    t3d_skeleton_use(&skel);
    t3d_matrix_set(&knightMatFP[frameIdx], true);
    t3d_model_draw_skinned(knightModel, &skel);
    //rspq_block_run(dplKnight);
}

void GO_NPCKnight::renderRdpq() {

}