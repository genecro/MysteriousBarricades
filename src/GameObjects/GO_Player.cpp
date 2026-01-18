//#include "GO_Player.h"
#include "../globals.h"
#include "../collision.h"

#define STICK_DEADZONE 8
#define INTERACTION_DISTANCE 1
#define INTERACTION_ANGLE T3D_PI/6.0f
#define PLAYER_WIDTH 2
#define FB_COUNT 3

GO_Player::GO_Player(std::string name) {
    name_ = name;
    position_ = {{0.0f,0.0f,0.0f}};
    moveDir = {{0,0,0}};
    rotation_ = 0.0f;
    objectWidth_ = 3.0f;
    downwardVel = 0.0f;
    terminalVel = objectWidth_ * 0.9f;
    grounded = true;

    //t3d_mat4_identity(&playerMat);
    //playerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    playerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);

    modelPlayer = t3d_model_load("rom:/N64Wizard.t3dm");
    cursorModel = t3d_model_load("rom:/cursor.t3dm");
    groundMarkerModel = t3d_model_load("rom:/groundMarker.t3dm");

    skel = t3d_skeleton_create_buffered(modelPlayer, FB_COUNT);
    skelBlend = t3d_skeleton_clone(&skel, false);
    animIdle = t3d_anim_create(modelPlayer, "Idle");
    t3d_anim_attach(&animIdle, &skel);
    animWalk = t3d_anim_create(modelPlayer, "Run");
    t3d_anim_attach(&animWalk, &skelBlend);
    frameIdx = 0;

    rspq_block_begin();
    t3d_model_draw_skinned(modelPlayer, &skel);
    dplPlayer = rspq_block_end();

    inventory_.items = new std::map<int, int>();

    playerState_ = PLAYER_STATE_BASE;

    t3d_mat4_identity(&cursorMat);
    t3d_mat4_identity(&cursorEdgeMat1);
    t3d_mat4_identity(&cursorEdgeMat2);
    cursorMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    cursorEdgeMatFP1 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    cursorEdgeMatFP2 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    t3d_mat4_identity(&groundMarkerMat);
    t3d_mat4_identity(&groundMarkerEdgeMat1);
    t3d_mat4_identity(&groundMarkerEdgeMat2);
    groundMarkerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    groundMarkerEdgeMatFP1 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    groundMarkerEdgeMatFP2 = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
}

GO_Player::~GO_Player() {
    t3d_model_free(modelPlayer);
    free_uncached(playerMatFP);
    t3d_skeleton_destroy(&skel);
    t3d_skeleton_destroy(&skelBlend);

    t3d_anim_destroy(&animIdle);
    t3d_anim_destroy(&animWalk);
}

void GO_Player::handleInput() {
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t rel = joypad_get_buttons_released(JOYPAD_PORT_1);

    if(btn.z) {
        playerState_ = PLAYER_STATE_CURSOR;
        animBlend = 0.0f;
    }
    else if(rel.z) playerState_ = PLAYER_STATE_BASE;

    switch(playerState_) {
        case PLAYER_STATE_BASE:
            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE ||
                joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {

                rotation_ = atan2f(joypad.stick_y, -joypad.stick_x) + 3.0f * T3D_PI / 4.0f;
                float speed = sqrt(pow(joypad.stick_x, 2) + pow(joypad.stick_y, 2)) * 0.006f;
                animBlend = speed/0.75f;
                if(animBlend > 1.0f) animBlend = 1.0f;
                //debugf("Speed: %.2f\n", speed);

                moveDir = {{
                    fm_cosf(rotation_) * speed * global::frameTimeMultiplier,
                    0.0f,
                    fm_sinf(rotation_) * speed * global::frameTimeMultiplier
                }};
            }

            /*if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE) {
                rotation_ += joypad.stick_x * 0.0007f * (global::frameTimeMultiplier);
            }

            if(joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                moveDir = {{
                    fm_cosf(rotation_) * (joypad.stick_y * 0.006f) * (global::frameTimeMultiplier), 
                    0.0f,
                    fm_sinf(rotation_) * (joypad.stick_y * 0.006f) * (global::frameTimeMultiplier)
                }};
            }*/
            else {
                moveDir = {{0,0,0}};
                animBlend = 0.0f;
            }

            if(joypad.btn.l) {
                downwardVel = 0.0f;
                moveDir += {{0, 0.5f*global::frameTimeMultiplier, 0}};
            }
            else {
                if(grounded) {
                    downwardVel = 0.0f;
                }
                else {
                    if(downwardVel < terminalVel) {
                        downwardVel += 0.1f;
                    }
                    moveDir.y -= downwardVel * global::frameTimeMultiplier;
                }
            }

            //moveDir = collision::resolveWallCollision(position_, fmaxf(objectWidth_, downwardVel*global::frameTimeMultiplier), global::gameState->collisionTris, T3D_PI/3.0f, moveDir, &grounded);
            moveDir = collision::resolveWallCollisionLoop(position_, fmaxf(objectWidth_, downwardVel*global::frameTimeMultiplier), global::gameState->collisionTris, T3D_PI/3.0f, moveDir, &grounded);
            
            position_ += moveDir;
        break;

        case PLAYER_STATE_CURSOR:
            if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE ||
                joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
                    float cursorRotation = atan2f(joypad.stick_y, -joypad.stick_x) + 3.0f * T3D_PI / 4.0f;
                    float magnitude = std::max(abs(joypad.stick_x), abs(joypad.stick_y)) / 75.0;
                    if(magnitude > 1.0f) magnitude = 1.0f;
                    cursorPos_ = position_+(T3DVec3){magnitude*10.0f*fm_cosf(cursorRotation), cursorHeight_, magnitude*10.0f*fm_sinf(cursorRotation)};
            }
            else {
                cursorPos_ = position_+(T3DVec3){0.0f, cursorHeight_, 0.0f};
            }

            if(btn.a) playerState_ = PLAYER_STATE_BARRICADE;
        break;

        case PLAYER_STATE_BARRICADE:
            if(rel.a) playerState_ = PLAYER_STATE_CURSOR;
        break;
    }

    
}

void GO_Player::update() {
    frameIdx = (frameIdx + 1) % FB_COUNT;

    cursorRotation_ += 0.007;

    if(rotation_ > T3D_PI) {
        rotation_ -= 2*T3D_PI;
    }

    else if(rotation_ < -T3D_PI) {
        rotation_ += 2*T3D_PI;
    }

    float deltaTime = 1.0f/60.0f * global::frameTimeMultiplier;
    t3d_anim_update(&animIdle, deltaTime);
    t3d_anim_set_speed(&animWalk, animBlend + 0.15f);
    t3d_anim_update(&animWalk, deltaTime);

    // We now blend the walk animation with the idle/attack one
    t3d_skeleton_blend(&skel, &skel, &skelBlend, animBlend);

    // Now recalc. the matrices, this will cause any model referencing them to use the new pose
    t3d_skeleton_update(&skel);
    
    /*
    t3d_mat4_from_srt_euler(&playerMat,
      (float[3]){0.15f, 0.15f, 0.15f},
      (float[3]){0.0f, rotation_, 0.0f},
      (position_-(T3DVec3){{0, objectWidth_, 0}}).v
    );
    t3d_mat4_to_fixed(playerMatFP, &playerMat);
    */

    t3d_mat4fp_from_srt_euler(&playerMatFP[frameIdx],
        (float[3]){0.05f, 0.05f, 0.05f},
        (float[3]){0.0f, rotation_ - T3D_PI / 2.0f, 0.0f},
        (position_-(T3DVec3){{0, objectWidth_, 0}}).v
    );

    if(playerState_ == PLAYER_STATE_CURSOR || playerState_ == PLAYER_STATE_BARRICADE) {
        t3d_mat4_from_srt_euler(&cursorMat,
            (float[3]){0.02f, 0.02f, 0.02f},
            (float[3]){0.0f, cursorRotation_, 0.0f},
            cursorPos_.v
        );
        t3d_mat4_to_fixed(cursorMatFP, &cursorMat);

        //groundMarkerPos = collision::findGroundIntersection(*collisionTris, position_);

        t3d_mat4_from_srt_euler(&groundMarkerMat,
            (float[3]){0.003f, 0.003f, 0.003f},
            (float[3]){0.0f, rotation_, 0.0f},
            (cursorPos_ - (T3DVec3){0.0f,cursorHeight_,0.0f}).v//groundMarkerPos.v
        );
        t3d_mat4_to_fixed(groundMarkerMatFP, &groundMarkerMat);
    }
}

void GO_Player::renderT3d() {
    
    t3d_skeleton_use(&skel);
    t3d_matrix_set(&playerMatFP[frameIdx], true);
    rspq_block_run(dplPlayer);

    if(playerState_ == PLAYER_STATE_CURSOR || playerState_ == PLAYER_STATE_BARRICADE) {
        rdpq_sync_pipe();

        rdpq_set_prim_color(cursorColor);
        t3d_matrix_set(cursorMatFP, true);
        t3d_model_draw(cursorModel);

        rdpq_set_prim_color(groundMarkerColor);
        t3d_matrix_set(groundMarkerMatFP, true);
        t3d_model_draw(groundMarkerModel);
    }
}

void GO_Player::renderRdpq() {
    
}

bool GO_Player::canInteract(T3DVec3 target, float targetWidth) {
    float playerAngle = rotation_+T3D_PI;
    if(playerAngle > T3D_PI) {
        playerAngle -= 2.0f*T3D_PI;
    }
    else if(playerAngle < -T3D_PI) {
        playerAngle += 2.0f*T3D_PI;
    }

    float plObjDiff = playerAngle - fm_atan2f(1.0f*(position_.z - target.z),1.0f*((position_.x - target.x)));

    if (plObjDiff > T3D_PI) {
        plObjDiff -= 2.0f*T3D_PI;
    }
    else if (plObjDiff < -T3D_PI) {
        plObjDiff += 2.0f*T3D_PI;
    }

    return t3d_vec3_distance2(target, position_) <= pow(PLAYER_WIDTH + targetWidth + INTERACTION_DISTANCE, 2) //is player close to target?
        && abs(plObjDiff) <= INTERACTION_ANGLE; //is player facing target?
}

bool GO_Player::isTouching(T3DVec3 target, float targetWidth) {
    return t3d_vec3_distance2({{target.x, 0, target.z}}, {{position_.x, 0, position_.z}}) <= pow(PLAYER_WIDTH + targetWidth, 2);
}

int GO_Player::removeItem(int id, int qty) {
    if(inventory_.items->count(id) > 0) {
        int diff = inventory_.items->at(id) - qty;
        if(diff > 0) {
            inventory_.items->at(id) = diff;
            return qty;
        }
        else {
            int numItems = inventory_.items->at(id);
            inventory_.items->erase(id);
            return numItems;
        }
    }
    return 0;
}

int GO_Player::addItem(int id, int qty) {
    if(inventory_.items->count(id) > 0) {
        inventory_.items->at(id) += qty;
        return qty;
    }
    else {
        inventory_.items->emplace(id, qty);
        return qty;
    }
}