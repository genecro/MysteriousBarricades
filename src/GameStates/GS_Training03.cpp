#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_Training03::GS_Training03(T3DVec3 startingCursorPosition) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);

    lightDirVec = {{-1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    envModel = t3d_model_load("rom:/training01.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/training01.bin");
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

    GO_RepairableTower* newTower = new GO_RepairableTower(
        (T3DVec3){0,0,8}, 
        100, 
        60, 
        (color_t){0x77, 0, 0xFF, 0xFF}, 
        -T3D_PI/2.0f,//-3.0f*T3D_PI/4.0f, 
        0);//-T3D_PI/4.0f);
    
    newTower->setRewardFunction([&](){
        global::gameState->nextStatePush = new GS_Training03InteriorA((T3DVec3){0,0,80});
    }, global::gameProgress.trainingRewardReceived);

    repairableList->push(newTower);

    enemyList->push(new GO_EnemyBasic((T3DVec3){4,0,2}, repairableList->getCurrRepairable()));
    enemyList->push(new GO_EnemyBasic((T3DVec3){-4,0,2}, repairableList->getCurrRepairable()));
    for(auto e : *enemyList->gameObjects_) {
        e->isMoving_ = false;
    }
    global::audioManager->playBGM(BGM_TRAINING, 0.4f);
    
}

GS_Training03::~GS_Training03() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete enemyList;
    delete barricadeList;
    delete theCursor;
    global::audioManager->clearSFX();
}

void GS_Training03::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //if(keys.start) {
    if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Training03>());
    }

    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Training03::update() {
    theCursor->update();
    updateCamera();

    if(!introFinished_ && global::GameInterruptStack->size() == 0) {
        introFinished_ = true;
        for(auto e : *enemyList->gameObjects_) {
            e->isMoving_ = true;
        }
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
    if(!endStateReached) checkForWinOrLoss();

    if(!introAlertShown) {
        introAlertShown = true;
        global::GameInterruptStack->push_back(
            (new GI_Alert("Training Tower, 3rd Floor:\nStructures", true, true))
            ->setNextInterrupt(
            (new GI_Alert("On this floor of the Training Tower,\nyou can practice defending\nand repairing a structure.", true))
            ->setNextInterrupt(
            (new GI_Alert("Enemies will advance toward the\nstructure. If they get too close,\nthey will start attacking it.", true))
            ->setNextInterrupt(
            (new GI_Alert("Cast barricades to prevent\nenemies from reaching the structure.", true))
            ->setNextInterrupt(
            (new GI_Alert("Hold the B button over the structure\nto repair it with your Repair Points.", true))
            ->setNextInterrupt(
            (new GI_Alert("Destroy both enemies to complete\nthis floor of the Training Tower!", true))
        ))))));
        global::GameInterruptStack->push_back(new GI_FadeIn(600));
    }

    if(!alertedStructureEntryOnce) {
        if(repairableList->getCurrRepairable()->fullyRepaired) {
            alertedStructureEntryOnce = true;
            global::GameInterruptStack->push_back(
                (new GI_Alert("Now that the structure is fully\nrepaired, press A to enter it.", false))
            );
        }
    }
}

void GS_Training03::renderT3d() {
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

void GS_Training03::renderRdpq() {
    objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    theCursor->renderRdpq();
}

void GS_Training03::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training03::handleInputCamera() {
    
}

void GS_Training03::updateCamera() {
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training03::levelWon() {
    /*
    global::GameInterruptStack->push_back((new GI_Alert("Structure training complete!", false))
        ->setNextInterrupt(
            (
                new GI_MultiChoice(         
                    "Retry", new GI_FadeToNextGS<GS_Training03>((T3DVec3){0,10,0}, 600.0f),
                    "Return to Level Select", new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f)
                )
            )
        )    
    );*/

    /*if(repairableList->repairables->at(0)->HPCurrent_ < repairableList->repairables->at(0)->HPTotal_) {
        global::GameInterruptStack->push_back((new GI_Alert("")))
    }*/

    global::GameInterruptStack->push_back((new GI_Alert("You have defeated the enemies!\nUse your repair points to repair\nthe structure.", false))->setNextInterrupt(
        new GI_Alert("Structures with a treasure icon\nabove them will allow entry\nwhen fully repaired.", false)
    ));
}

void GS_Training03::levelLost() {
    global::GameInterruptStack->push_back((new GI_Alert("The structure has fallen!\nThe enemies have prevailed.", false))
        ->setNextInterrupt(
            (new GI_Alert("Please try this training floor again.", false))
        ->setNextInterrupt(
            (new GI_FadeToNextGS<GS_Training03>((T3DVec3){0,10,0}, 600.0f))
        )));
}

void GS_Training03::checkForWinOrLoss() {
    //if(enemiesDestroyed >= 2) {
    if(enemyList->gameObjects_->size() == 0) {
    //if(repairableList->repairables->at(0)->HPCurrent_ >= repairableList->repairables->at(0)->HPTotal_ || theCursor->RPCurrent_ <= 0) {
        endStateReached = true;
        levelWon();
        return;
    }
    for(const auto& r : *repairableList->repairables) {
        if(r->HPCurrent_ <= 0) {
            endStateReached = true;
            levelLost();
            break;
        }
    }
}

void GS_Training03::barricadeCastFailed() {
    if(!barricadeHasFailedOnce) {
        barricadeHasFailedOnce = true;
        global::GameInterruptStack->push_back(new GI_Alert("Be careful! If an enemy is in\nthe way when casting a barricade,\nit won't materialize!", true));
    }
}

void GS_Training03::enemiesAttackingStructure() {
    if(!alertedEnemiesAttackingOnce) {
        alertedEnemiesAttackingOnce = true;

        global::GameInterruptStack->push_back((new GI_Alert("Enemies have started attacking\nthe structure! Move onto it\nand hold B to repel them.", true)));
    }
}

void GS_Training03::enemyDestroyed() {
    if(!alertedRPBoostOnce) {
        alertedRPBoostOnce = true;

        global::GameInterruptStack->push_back((new GI_Alert("Enemies drop Repair Points (RP)\nwhen defeated. Pick them up and\nuse them to repair structures.", false)));
    }
}