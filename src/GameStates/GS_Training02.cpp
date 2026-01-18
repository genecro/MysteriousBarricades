#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_Training02::GS_Training02(T3DVec3 startingCursorPosition) {
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

    enemyList->push(new GO_EnemyBasic((T3DVec3){8,0,-8}, (T3DVec3){0,0,8}, false));
    enemyList->push(new GO_EnemyBasic((T3DVec3){0,0,8}, (T3DVec3){-8,0,-8}, false));
    enemyList->push(new GO_EnemyBasic((T3DVec3){-8,0,-8}, (T3DVec3){8,0,-8}, false));

    for(auto e : *enemyList->gameObjects_) {
        e->isMoving_ = false;
    }

    global::GameInterruptStack->push_back(
            (new GI_Alert("Training Tower, 2nd Floor:\nEnemies", true, true))
            ->setNextInterrupt(
            (new GI_Alert("On this floor of the tower, you\ncan practice destroying enemies.", true))
            ->setNextInterrupt(
            (new GI_Alert("You cannot attack enemies directly.\nYou must place barricades in\nfront of them.", true))
            ->setNextInterrupt(
            (new GI_Alert("An enemy will be damaged, knocked\nback, and stunnned when it makes\ncontact with your barricade.", true))
            ->setNextInterrupt(
            (new GI_Alert("Destroy all 3 enemies to complete\nthis floor of the Training Tower!", true))
        )))));

    global::GameInterruptStack->push_back(new GI_FadeIn(600));
}

GS_Training02::~GS_Training02() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete enemyList;
    delete theCursor;
}

void GS_Training02::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //if(keys.start) {
    if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Training02>());
    }

    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Training02::update() {
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
}

void GS_Training02::renderT3d() {
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

void GS_Training02::renderRdpq() {
    objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    theCursor->renderRdpq();
}

void GS_Training02::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training02::handleInputCamera() {
    
}

void GS_Training02::updateCamera() {
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training02::levelWon() {
    global::GameInterruptStack->push_back((new GI_Alert("Enemy training complete!", false))
        ->setNextInterrupt(
            (
                new GI_MultiChoice(
                    "Next", new GI_FadeToNextGS<GS_Training03>((T3DVec3){0,10,0}, 600.0f),            
                    "Retry", new GI_FadeToNextGS<GS_Training02>((T3DVec3){0,10,0}, 600.0f),
                    "Quit", new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f)
                )
            )
        )    
    );
}

void GS_Training02::checkForWinOrLoss() {
    if(enemiesDestroyed >= 3) {
        endStateReached = true;
        levelWon();
    }
}

void GS_Training02::enemyDestroyed() {
    enemiesDestroyed++;
}

void GS_Training02::barricadeCastFailed() {
    if(!barricadeHasFailedOnce) {
        barricadeHasFailedOnce = true;
        global::GameInterruptStack->push_back(new GI_Alert("Be careful! If an enemy is in\nthe way when casting a barricade,\nit won't materialize!", false));
    }
}