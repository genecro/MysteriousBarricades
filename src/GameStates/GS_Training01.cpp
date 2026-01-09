#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_Training01::GS_Training01(T3DVec3 startingCursorPosition) {
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

    global::GameInterruptStack->push_back(
            (new GI_Alert("Training Tower, 1st Floor:\nBarricades", true))
            ->setNextInterrupt(
            (new GI_Alert("On this floor of the tower, you can\npractice casting barricades."))
            ->setNextInterrupt(
            (new GI_Alert("Hold the A button and tilt the\ncontrol stick."))
            ->setNextInterrupt(
            (new GI_Alert("Release the A button to cast a\nbarricade aligned with the\nfloating cursors."))
            ->setNextInterrupt(
            (new GI_Alert("Create 10 barricades to complete\nthis floor of the Training Tower!"))
        )))));
    
    global::GameInterruptStack->push_back(new GI_FadeIn(600));
}

GS_Training01::~GS_Training01() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete barricadeList;
    delete enemyList;
    delete theCursor;
}

void GS_Training01::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //if(keys.start) {
    if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Training01>());
    }

    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Training01::update() {
    theCursor->update();
    updateCamera();

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

void GS_Training01::renderT3d() {
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

void GS_Training01::renderRdpq() {
    objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    theCursor->renderRdpq();
}

void GS_Training01::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training01::handleInputCamera() {
    
}

void GS_Training01::updateCamera() {
    camera.target = theCursor->position_ + (T3DVec3){0, -3, 0};
    camera.pos = theCursor->position_ + (T3DVec3){0, 10, -15};
}

void GS_Training01::levelWon() {
    //enemyList->destroyAllEnemies();
    global::GameInterruptStack->push_back((new GI_Alert("Barricade training complete!"))
        ->setNextInterrupt(
            (
                new GI_MultiChoice(
                    "Next", new GI_FadeToNextGS<GS_Training02>((T3DVec3){0,10,0}, 600.0f),            
                    "Retry", new GI_FadeToNextGS<GS_Training01>((T3DVec3){0,10,0}, 600.0f),
                    "Quit", new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f)
                )
            )
        )    
    );
}

void GS_Training01::levelLost() {
    global::GameInterruptStack->push_back(new GI_Alert("You lost!"));
}

void GS_Training01::checkForWinOrLoss() {
    if(barricadesCreated >= 10) {
        endStateReached = true;
        levelWon();
    }
}

void GS_Training01::barricadeCreated() {
    barricadesCreated++;
}

void GS_Training01::triedToCastWithoutSlots() {
    if(!slotsAlertHasBeenShown) {
    global::GameInterruptStack->push_back((new GI_Alert("You can only have " + std::to_string(global::gameProgress.numBarricades) + " active\nbarricades at one time."))
                            ->setNextInterrupt(new GI_Alert("Keep an eye on your barricade\ntracker at the top of the screen.")));
    slotsAlertHasBeenShown = true;
    }
}