#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_Training04::GS_Training04(T3DVec3 startingCursorPosition) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);

    lightDirVec = {{-1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    envModel = t3d_model_load("rom:/training04.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/training04.bin");
    collision::scaleTriangles(&collisionTris, scaleFactor);

    envModel->aabbMax[0]*=scaleFactor;
    envModel->aabbMin[0]*=scaleFactor;
    envModel->aabbMax[2]*=scaleFactor;
    envModel->aabbMin[2]*=scaleFactor;

    t3d_viewport_set_projection(viewport, 75.0f, 20.0f, 200.0f);

    theCursor = new GO_Cursor(startingCursorPosition, &collisionTris);

    initCamera();

    objectList = new GameObjectList();
    repairableList = new RepairableList();
    barricadeList = new BarricadeList();
    enemyList = new EnemyList(&collisionTris);

    

    /*
    global::GameInterruptStack->push_back(
            (new GI_Alert("Training Tower, 4th Floor:\nProjectiles", true, true))
            ->setNextInterrupt(
            (new GI_Alert("Test 02", true))
            ->setNextInterrupt(
            (new GI_Alert("Test 03", true))
        )));
        */

    timelineCtr = 0;

    projectilesDestroyBarricades = true;

    addRandomCannon.push_back({10,[](){return true;}});
    addRandomCannon.push_back({2*60, [&](){
        int randRotPos = rand() % 3;
        int randColor = rand() % 3;
        currCannon = new GO_Cannon(cannonPlacement[randRotPos].pos, cannonPlacement[randRotPos].rot, -17, 17, colors[randColor], colors[randColor]);
        currCannon->active_ = false;
        objectList->push(currCannon);
        return true;
    }});
    addRandomCannon.push_back({2*60, [&](){
        currCannon->shootProjectile();
        return true;
    }});
    addRandomCannon.push_back({2*60, [&](){
        currCannon->timeToDelete = true;
        currTimeline = addRandomCannon;
        return true;
    }});
    currTimeline = addRandomCannon;

    global::audioManager->playBGM(BGM_MAIN_MENU, 0.8f);
}

GS_Training04::~GS_Training04() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete enemyList;
}

void GS_Training04::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //if(keys.start) {
    if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Training04>());
    }

    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Training04::update() {
    theCursor->update();
    updateCamera();
    if(!endStateReached) {
        checkForWinOrLoss();
        updateTimeline();
    }

    t3d_mat4_from_srt_euler(&envMat,
        (float[3]){ scaleFactor, scaleFactor, scaleFactor},
        (float[3]){0.0f, 0.0f, 0.0f},
        (float[3]){0,0,0}
    );
    t3d_mat4_to_fixed(envMatFP, &envMat);

    objectList->update(theCursor->groundMarkerPos);
    enemyBarricadeCheck();
    enemyRepairableCheck();
    repairableList->update();
    barricadeList->update();
    enemyList->update();
    
    projectileBarricadeCheck();
}

void GS_Training04::renderT3d() {
    t3d_viewport_set_projection(viewport, camera.FOV, 10.0f, 200.0f);
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(160, 110, 200, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    t3d_viewport_look_at(viewport, camera.pos, camera.target, {{0, 1, 0}});

    t3d_matrix_push_pos(1);
    
    repairableList->renderT3d();
    enemyList->renderT3d();
    
    t3d_matrix_set(envMatFP, true);
    t3d_model_draw(envModel);

    barricadeList->renderT3d();
    objectList->renderT3d();
    theCursor->renderT3d();
    t3d_matrix_pop(1);
}

void GS_Training04::renderRdpq() {
    objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    theCursor->renderRdpq();

    if(!endStateReached) {
        std::string currStr = "Current score: " + std::to_string(currentScore);
        std::string highStr = "High score: " + std::to_string(global::gameProgress.challenge1HighScore);

        rdpq_text_printf(&(rdpq_textparms_t) {
                .style_id= FONTSTYLE_WHITE,
            }, 
            FONT_PIACEVOLI_16, 
            25, 
            display_get_height()-50, 
            (currStr + "\n" + highStr).c_str()
        );
    }
}

void GS_Training04::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training04::handleInputCamera() {
    
}

void GS_Training04::updateCamera() {
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training04::levelWon() {
    enemyList->destroyAllEnemies();
    global::GameInterruptStack->push_back(new GI_Alert("You won!", false));
}

void GS_Training04::levelLost() {
    endStateReached = true;
    global::GameInterruptStack->push_back((new GI_Alert("Game over!", false))
        ->setNextInterrupt(
            (new GI_Alert("High score: " + std::to_string(global::gameProgress.challenge1HighScore), false))
        ->setNextInterrupt(
            new GI_MultiChoice("Retry", new GI_FadeToNextGS<GS_Training04>((T3DVec3){0,10,0}, 600.0f),
            "Quit to Level Select", new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f))
        )));
}

void GS_Training04::checkForWinOrLoss() {
    for(GameObject * p : *objectList->gameObjects) {
        if(p->isProjectile_ && dynamic_cast<GO_Projectile*>(p)->reflected) {
            float wallRot = 0.0f;
            T3DVec3 wallPos = {0,0,0};
            float L2 = 7.5;
            color_t projectileColor = dynamic_cast<GO_Projectile*>(p)->objColor_;
            switch(projectileColor.r) {
                case 0xFF:
                    switch(projectileColor.g) {
                        case 0xFF: //yellow
                            wallRot = cannonPlacement[2].rot;
                            wallPos = cannonPlacement[2].pos;
                        break;

                        case 0: //red
                            wallRot = cannonPlacement[1].rot;
                            wallPos = cannonPlacement[1].pos;
                        break;
                    }
                break;

                case 0: //blue
                    wallRot = cannonPlacement[0].rot;
                    wallPos = cannonPlacement[0].pos;
                    L2 = 5;
                break;
            }
            float xp = (p->position_.x - wallPos.x)*fm_cosf(wallRot) + (p->position_.z - wallPos.z)*fm_sinf(wallRot);
            float zp = -(p->position_.x - wallPos.x)*fm_sinf(wallRot) + (p->position_.z - wallPos.z)*fm_cosf(wallRot);
            
            float xclamp = xp < -L2 ? -L2 : xp > L2 ? L2 : xp;//clamp(xp, -L2, L2);
            
            float distance = (p->objectWidth_ + p->speed_) * global::frameTimeMultiplier;
            if((xp - xclamp)*(xp - xclamp) + zp*zp <= distance*distance) {
                currentScore++;
                if(currentScore > global::gameProgress.challenge1HighScore) {
                    global::gameProgress.challenge1HighScore = currentScore;
                }
                p->timeToDelete = true;
            }
        }
    }
    
}
void GS_Training04::updateTimeline() {
    timelineCtr += global::frameTimeMultiplier;
    if(!currTimeline.empty() && timelineCtr >= currTimeline.front().time) {
        if(currTimeline.front().action()) {
            currTimeline.erase(currTimeline.begin());
        }
        else {
            currTimeline.front().time = 5*60; // try again in 5 seconds
        }
        timelineCtr = 0;
    }
}

void GS_Training04::projectileWentOutOfBounds() {
    levelLost();

}