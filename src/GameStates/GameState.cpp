#include "GameState.h"
//#include "../GameObjects/GO.h"
//#include "../GameInterrupts/GI.h"
#include "../globals.h"
//#include "GameObject.h"

#define CAMERA_ROTATE_SPEED 0.05f
#define CAMERA_DISTANCE_SPEED 5.0f
#define CAMERA_HEIGHT_SPEED 5.0f
#define CAMERA_FOV_SPEED 0.05f

GameState::GameState()
{

}

GameState::~GameState()
{
    
}

void GameState::initCamera() {
    constants.midHeightFromPlayer = 40;
    constants.midDistanceFromPlayer = 60;
    constants.midFOV = 0.128*T3D_PI;
    constants.closeHeightFromPlayer = 10;
    constants.closeDistanceFromPlayer = 40;
    constants.closeFOV = 0.212*T3D_PI;
    constants.cRotAngle = 0.25*T3D_PI;

    camera.targetDistanceFromPlayer = constants.midDistanceFromPlayer;
    camera.distanceFromPlayer = constants.midDistanceFromPlayer;
    camera.targetHeightFromPlayer = constants.midHeightFromPlayer;
    camera.heightFromPlayer = constants.midHeightFromPlayer;
    camera.targetFOV = constants.midFOV;
    camera.FOV = constants.midFOV;
    camera.rotAngle = 0;
    camera.targetRotAngle = camera.rotAngle;

    endTime = get_ticks();
}

void GameState::handleInputCamera() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    
    if(keys.c_left) {
        if(global::gameState->thePlayer_->camState_.followPlayerRot) {
            global::gameState->thePlayer_->camState_.followPlayerRot = false;
            camera.targetRotAngle = round(camera.rotAngle * 4.0 / T3D_PI) * T3D_PI * 0.25;
        }
        camera.targetRotAngle -= constants.cRotAngle;
    }

    if(keys.c_right) {
        if(global::gameState->thePlayer_->camState_.followPlayerRot) {
            global::gameState->thePlayer_->camState_.followPlayerRot = false;
            camera.targetRotAngle = round(camera.rotAngle * 4.0 / T3D_PI) * T3D_PI * 0.25;
        }
        camera.targetRotAngle += constants.cRotAngle;
    }

    if(keys.r) {
        global::gameState->thePlayer_->camState_.followPlayerRot = !global::gameState->thePlayer_->camState_.followPlayerRot;
        if(!global::gameState->thePlayer_->camState_.followPlayerRot) {
            camera.targetRotAngle = round(camera.rotAngle * 4.0 / T3D_PI) * T3D_PI * 0.25;
        }
        else {
            camera.rotAngle = -global::gameState->thePlayer_->rotation_;
        }
    }

    if(keys.c_down && global::gameState->thePlayer_->camState_.closeness < CAM_MID) {
        global::gameState->thePlayer_->camState_.closeness++;
        if(global::gameState->thePlayer_->camState_.closeness==CAM_MID) {
            camera.targetDistanceFromPlayer=constants.midDistanceFromPlayer;
            camera.targetHeightFromPlayer=constants.midHeightFromPlayer;
            camera.targetFOV = constants.midFOV;
        }
    }

    if(keys.c_up && global::gameState->thePlayer_->camState_.closeness > CAM_CLOSE) {
        global::gameState->thePlayer_->camState_.closeness--;
        if(global::gameState->thePlayer_->camState_.closeness==CAM_CLOSE) {
            camera.targetDistanceFromPlayer=constants.closeDistanceFromPlayer;
            camera.targetHeightFromPlayer=constants.closeHeightFromPlayer;
            camera.targetFOV = constants.closeFOV;
        }
    }
}

void GameState::updateCamera() {
    
    if(camera.rotAngle < camera.targetRotAngle) {
        camera.rotAngle += CAMERA_ROTATE_SPEED * global::frameTimeMultiplier;
        if(camera.rotAngle > camera.targetRotAngle) {
            if(camera.targetRotAngle >= 2.0f*T3D_PI) {
                camera.targetRotAngle -= 2.0f*T3D_PI;
            }
            camera.rotAngle = camera.targetRotAngle;
        }
    }
    else if(camera.rotAngle > camera.targetRotAngle) {
        camera.rotAngle -= CAMERA_ROTATE_SPEED * global::frameTimeMultiplier;
        if(camera.rotAngle < camera.targetRotAngle) {
            if(camera.targetRotAngle < 0) {
                camera.targetRotAngle += 2.0f*T3D_PI;
            }
            camera.rotAngle = camera.targetRotAngle;
        }
    }

    if(camera.distanceFromPlayer < camera.targetDistanceFromPlayer) {
        camera.distanceFromPlayer += CAMERA_DISTANCE_SPEED * global::frameTimeMultiplier;
        if(camera.distanceFromPlayer > camera.targetDistanceFromPlayer) {
            camera.distanceFromPlayer = camera.targetDistanceFromPlayer;
        }
    }

    else if(camera.distanceFromPlayer > camera.targetDistanceFromPlayer) {
        camera.distanceFromPlayer -= CAMERA_DISTANCE_SPEED * global::frameTimeMultiplier;
        if(camera.distanceFromPlayer < camera.targetDistanceFromPlayer) {
            camera.distanceFromPlayer = camera.targetDistanceFromPlayer;
        }
    }

    if(camera.heightFromPlayer < camera.targetHeightFromPlayer) {
        camera.heightFromPlayer += CAMERA_HEIGHT_SPEED * global::frameTimeMultiplier;
        if(camera.heightFromPlayer > camera.targetHeightFromPlayer) {
            camera.heightFromPlayer = camera.targetHeightFromPlayer;
        }
    }

    else if(camera.heightFromPlayer > camera.targetHeightFromPlayer) {
        camera.heightFromPlayer -= CAMERA_HEIGHT_SPEED * global::frameTimeMultiplier;
        if(camera.heightFromPlayer < camera.targetHeightFromPlayer) {
            camera.heightFromPlayer = camera.targetHeightFromPlayer;
        }
    }

    if(camera.FOV < camera.targetFOV) {
        camera.FOV += CAMERA_FOV_SPEED * global::frameTimeMultiplier;
        if(camera.FOV > camera.targetFOV) {
            camera.FOV = camera.targetFOV;
        }
    }

    else if(camera.FOV > camera.targetFOV) {
        camera.FOV -= CAMERA_FOV_SPEED * global::frameTimeMultiplier;
        if(camera.FOV < camera.targetFOV) {
            camera.FOV = camera.targetFOV;
        }
    }

    if(global::gameState->thePlayer_->camState_.followPlayerRot){
        camera.rotAngle = -global::gameState->thePlayer_->rotation_ - 0.5*T3D_PI;
    }
    
    camera.pos = {{
        global::gameState->thePlayer_->position_.x + camera.distanceFromPlayer * fm_sinf(camera.rotAngle),
        global::gameState->thePlayer_->position_.y + camera.heightFromPlayer,
        global::gameState->thePlayer_->position_.z + camera.distanceFromPlayer * fm_cosf(camera.rotAngle)
    }};

    camera.target = global::gameState->thePlayer_->position_ - (T3DVec3){{0, global::gameState->thePlayer_->objectWidth_, 0}};

    if(global::gameState->thePlayer_->camState_.closeness==CAM_MID) {
        camera.target += {{0,5,0}};
    }

    if(global::gameState->thePlayer_->camState_.closeness==CAM_CLOSE) {
        camera.target += {{0,10,0}};
    }
}

void GameState::projectileBarricadeCheck() {
    for(GameObject* p : *(objectList->gameObjects)) {
        if(p->isProjectile_) {
            for(GO_Barricade* b: *(barricadeList->gameObjects_)) {
                b->processProjectile((GO_Projectile*)p);
            }
        }
    }
}

void GameState::projectileEnemyCheck() {
    for(GameObject* p : *(objectList->gameObjects)) {
        if(p->isProjectile_) {
            for(GO_Enemy* e: *(enemyList->gameObjects_)) {
                e->processProjectile((GO_Projectile*)p);
            }
        }
    }
}

void GameState::projectileRepairableCheck() {
    for(GameObject* p : *(objectList->gameObjects)) {
        if(p->isProjectile_) {
            for(GO_Repairable* r: *(repairableList->repairables)) {
                r->processProjectile((GO_Projectile*)p);
            }
        }
    }
}

void GameState::enemyBarricadeCheck() {
    for(GO_Barricade* b: *(barricadeList->gameObjects_)) {
        for(GO_Enemy* e: *(enemyList->gameObjects_)) {
            b->processEnemy(e);
        }
        if(!b->castSuccess_ && !b->castHasFailed_) {
            b->castSuccess();
        }
    }
}

void GameState::enemyRepairableCheck() {
    for(GO_Repairable* r: *(repairableList->repairables)) {
        for(GO_Enemy* e: *(enemyList->gameObjects_)) {
            if(e->getState() != global::ENEMY_STATE_ATTACKING && e->getState() != global::ENEMY_STATE_CHASING_CURSOR && t3d_vec3_distance2(e->position_, r->position_) <= pow(e->objectWidth_+r->objectWidth_, 2)) {
                e->setStateAttacking(r);
            }
        }
    }
}

void GameState::updateTimeline() {
    timelineCtr += global::frameTimeMultiplier;
    if(!timeline.empty() && timelineCtr >= timeline.front().time) {
        if(timeline.front().action()) {
            timeline.erase(timeline.begin());
        }
        else {
            timeline.front().time = 5*60; // try again in 5 seconds
        }
        timelineCtr = 0;
    }
}

void GameState::checkForWinOrLoss() {

}
void GameState::levelWon() {

}
void GameState::levelLost() {
    
}

void GameState::barricadeCreated() {

}

void GameState::enemyDestroyed() {
    
}

void GameState::barricadeCastFailed() {
    
}

void GameState::triedToCastWithoutSlots() {

}

void GameState::enemiesAttackingStructure() {

}

void GameState::projectileWentOutOfBounds() {

}

void GameState::projectileNotDeflected() {
    
}