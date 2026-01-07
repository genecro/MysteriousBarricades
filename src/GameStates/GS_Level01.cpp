#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_Level01::GS_Level01(T3DVec3 startingCursorPosition) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);

    lightDirVec = {{-1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    envModel = t3d_model_load("rom:/level01.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/level01.bin");
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
    repairableList->push(new GO_RepairableTower(collision::findGroundIntersection(collisionTris, (T3DVec3){15,10,15}), 100, 25, (color_t){255, 255, 0, 255}, -T3D_PI/2.0f, 0));
    repairableList->push(new GO_RepairableTower(collision::findGroundIntersection(collisionTris, (T3DVec3){-15,10,-15}), 100, 25, (color_t){120, 0, 255, 255}, T3D_PI/2.0f, T3D_PI));

    barricadeList = new BarricadeList();

    enemyList = new EnemyList(&collisionTris);



    enemyList->push(new GO_EnemyBasic((T3DVec3){-15, 5, 0}, repairableList->getRandDamagedRep()));
    enemyList->push(new GO_EnemyBasic((T3DVec3){15, 5, -15}, repairableList->getRandDamagedRep()));

    remainingEnemies = 16;

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 6 || endStateReached) return false;
            enemyList->push(new GO_EnemyBasic((T3DVec3){0, 5, -15}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){-15, 5, 15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 2;
            return true;
        }
    });

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 6 || endStateReached) return false;
            enemyList->push(new GO_EnemyBasic((T3DVec3){-7, 5, 0}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyBasic((T3DVec3){8, 5, -15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 2;
            return true;
        }
    });

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 6 || endStateReached) return false;
            enemyList->push(new GO_EnemyBasic((T3DVec3){0, 5, -8}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){-7, 5, 15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 2;
            return true;
        }
    });

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 5 || endStateReached) return false;
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){-15, 5, 0}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){15, 5, -15}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){0, 5, -15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 3;
            return true;
        }
    });

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 5 || endStateReached) return false;
            enemyList->push(new GO_EnemyBasic((T3DVec3){-15, 5, 15}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyBasic((T3DVec3){-7, 5, 0}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyBasic((T3DVec3){8, 5, -15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 3;
            return true;
        }
    });

    timeline.push_back({20*60,
        [&](){
            if(enemyList->gameObjects_->size() >= 4 || endStateReached) return false;
            enemyList->push(new GO_EnemyBasic((T3DVec3){0, 5, -8}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyRushToBarricade((T3DVec3){-7, 5, 15}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyBasic((T3DVec3){-15, 5, 0}, repairableList->getRandDamagedRep()));
            enemyList->push(new GO_EnemyBasic((T3DVec3){15, 5, -15}, repairableList->getRandDamagedRep()));
            remainingEnemies -= 4;
            return true;
        }
    });
}

GS_Level01::~GS_Level01() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete enemyList;
}

void GS_Level01::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //if(keys.start) {
    if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause());
    }

    if(btn.c_up) {
        objectList->push(new GO_Projectile((T3DVec3){0,10,0}, 3.0f*T3D_PI/4.0f, 0.3f));
    }

    float borderScale = 0.8f;

    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Level01::update() {
    repairableList->update();
    theCursor->update();
    updateCamera();
    updateTimeline();

    t3d_mat4_from_srt_euler(&envMat,
        (float[3]){ scaleFactor, scaleFactor, scaleFactor},
        (float[3]){0.0f, 0.0f, 0.0f},
        (float[3]){0,0,0}
    );
    t3d_mat4_to_fixed(envMatFP, &envMat);

    objectList->update(theCursor->groundMarkerPos);
    enemyBarricadeCheck();
    barricadeList->update();
    enemyList->update();
    //enemyRepairableCheck();
    if(!endStateReached) checkForWinOrLoss();
}

void GS_Level01::renderT3d() {
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

void GS_Level01::renderRdpq() {
    objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    theCursor->renderRdpq();

    std::string enRemStr = "Enemies Remaining: " + std::to_string(remainingEnemies + enemyList->gameObjects_->size());

    rdpq_text_printf(&(rdpq_textparms_t) {
            .style_id= FONTSTYLE_WHITE,
        }, 
        FONT_PIACEVOLI_16, 
        25, 
        display_get_height()-25, 
        enRemStr.c_str()
    );
}

void GS_Level01::testFunc() {
    debugf("Test func Level01\n");
}


void GS_Level01::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Level01::handleInputCamera() {
    
}

void GS_Level01::updateCamera() {
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Level01::levelWon() {
    enemyList->destroyAllEnemies();
    global::GameInterruptStack->push_back(new GI_Alert("You won!"));
}

void GS_Level01::levelLost() {
    global::GameInterruptStack->push_back(new GI_Alert("You lost!"));
}

void GS_Level01::checkForWinOrLoss() {
    bool allFullyRepaired = true;
    for(GO_Repairable* r: *repairableList->repairables) {
        if(!r->fullyRepaired) {
            allFullyRepaired = false;
        }
        if(r->HPCurrent_<=0) {
            endStateReached = true;
            levelLost();
            break;
        }
    }

    if(allFullyRepaired) {
        //win
        endStateReached = true;
        levelWon();
    }
}