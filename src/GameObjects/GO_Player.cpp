//#include "GO_Player.h"
#include "../globals.h"
#include "../collision.h"

#define STICK_DEADZONE 8
#define INTERACTION_DISTANCE 1
#define INTERACTION_ANGLE T3D_PI/6.0f
#define PLAYER_WIDTH 2

GO_Player::GO_Player(std::string name) {
    name_ = name;
    position_ = {{0.0f,0.0f,0.0f}};
    moveDir = {{0,0,0}};
    rotation_ = 0.0f;
    objectWidth_ = 3.0f;
    downwardVel = 0.0f;
    terminalVel = objectWidth_ * 0.9f;
    grounded = true;

    t3d_mat4_identity(&playerMat);
    playerMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    modelPlayer = t3d_model_load("rom:/playerTemp.t3dm");
    rspq_block_begin();
    t3d_model_draw(modelPlayer);
    dplPlayer = rspq_block_end();

    inventory_.items = new std::map<int, int>();
}

GO_Player::~GO_Player() {
    t3d_model_free(modelPlayer);
    free_uncached(playerMatFP);
}

void GO_Player::handleInput() {
    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(joypad.stick_x < -STICK_DEADZONE || joypad.stick_x > STICK_DEADZONE) {
        rotation_ += joypad.stick_x * 0.0007f * (global::frameTimeMultiplier);
    }

    if(joypad.stick_y < -STICK_DEADZONE || joypad.stick_y > STICK_DEADZONE) {
        moveDir = {{
            fm_cosf(rotation_) * (joypad.stick_y * 0.006f) * (global::frameTimeMultiplier), 
            0.0f,
            fm_sinf(rotation_) * (joypad.stick_y * 0.006f) * (global::frameTimeMultiplier)
        }};
    }
    else {
        moveDir = {{0,0,0}};
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
}

void GO_Player::update() {
    

    if(rotation_ > T3D_PI) {
        rotation_ -= 2*T3D_PI;
    }

    else if(rotation_ < -T3D_PI) {
        rotation_ += 2*T3D_PI;
    }
    
    t3d_mat4_from_srt_euler(&playerMat,
      (float[3]){0.05f, 0.05f, 0.05f},
      (float[3]){0.0f, rotation_, 0.0f},
      (position_-(T3DVec3){{0, objectWidth_, 0}}).v
    );
    t3d_mat4_to_fixed(playerMatFP, &playerMat);
}

void GO_Player::renderT3d() {
    t3d_matrix_set(playerMatFP, true);
    rspq_block_run(dplPlayer);
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