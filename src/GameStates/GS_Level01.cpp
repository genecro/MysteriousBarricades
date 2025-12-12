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
    collision::scaleTriangles(&collisionTris, scaleFactor);//3.0f/5.0f);

    t3d_viewport_set_projection(viewport, 75.0f, 20.0f, 200.0f);

    //global::thePlayer->position_ = startingPlayerPos + (T3DVec3){{0, global::thePlayer->objectWidth_, 0}};

    theCursor = new GO_Cursor(startingCursorPosition, &collisionTris);

    initCamera();

    objectList = new GameObjectList();
    //objectList->push(new GO_Door<GS_001ChurchSquare>((T3DVec3){{-9,0,-17}}, 0.5f*T3D_PI, 4, GS_001ChurchSquare::startPos.SHOP_INTERIOR));
    repairableList = new RepairableList();
    repairableList->push(new GO_RepairableTower(collision::findGroundIntersection(collisionTris, (T3DVec3){15,10,15}), 100, 25, (color_t){255, 255, 0, 255}));
    repairableList->push(new GO_RepairableTower(collision::findGroundIntersection(collisionTris, (T3DVec3){-15,10,-15}), 100, 25, (color_t){120, 0, 255, 255}));

    barricadeList = new BarricadeList();

    enemyList = new EnemyList(&collisionTris);
    enemyList->push(new GO_EnemyBasic((T3DVec3){-15, 5, 0}, repairableList->getCurrRepairable()->position_));
    enemyList->push(new GO_EnemyBasic((T3DVec3){15, 5, -15}, repairableList->getCurrRepairable()->position_));
    enemyList->push(new GO_EnemyBasic((T3DVec3){0, 5, -15}, repairableList->getCurrRepairable()->position_));
    enemyList->push(new GO_EnemyBasic((T3DVec3){-15, 5, 15}, repairableList->getCurrRepairable()->position_));
}

GS_Level01::~GS_Level01() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
}

void GS_Level01::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(keys.start) {
        global::GameInterruptStack->push_back(new GI_Pause());
    }

    if(keys.b) {
        
    }

    
    float borderScale = 0.8f;

    theCursor->handleInput();

    /*
    if(theCursor->position_.x > envModel->aabbMax[0]*scaleFactor*borderScale) {
        theCursor->position_.x = envModel->aabbMax[0]*scaleFactor*borderScale;
    }
    else if (theCursor->position_.x < envModel->aabbMin[0]*scaleFactor*borderScale) {
        theCursor->position_.x = envModel->aabbMin[0]*scaleFactor*borderScale;
    }

    if(theCursor->position_.z > envModel->aabbMax[2]*scaleFactor*borderScale) {
        theCursor->position_.z = envModel->aabbMax[2]*scaleFactor*borderScale;
    }
    else if (theCursor->position_.z < envModel->aabbMin[2]*scaleFactor*borderScale) {
        theCursor->position_.z = envModel->aabbMin[2]*scaleFactor*borderScale;
    }
        */

    //global::thePlayer->handleInput();
    handleInputCamera();
    //objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
}

void GS_Level01::update() {
    //global::thePlayer->update();
    theCursor->update();
    //theCursor->groundMarkerPos = collision::findGroundIntersection(collisionTris, theCursor->position_);//+(T3DVec3){0,3,0};
    //debugf("Cursor:        x=%0.2f, y=%0.2f, z=%0.2f\n", theCursor->position_.x, theCursor->position_.y, theCursor->position_.z);
    //debugf("Ground marker: x=%0.2f, y=%0.2f, z=%0.2f\n\n", theCursor->groundMarkerPos.x, theCursor->groundMarkerPos.y, theCursor->groundMarkerPos.z);
    updateCamera();

    

    t3d_mat4_from_srt_euler(&envMat,
        (float[3]){ scaleFactor, scaleFactor, scaleFactor},
        (float[3]){0.0f, 0.0f, 0.0f},
        (float[3]){0,0,0}
    );
    t3d_mat4_to_fixed(envMatFP, &envMat);

    //objectList->update();
    repairableList->update();
    barricadeList->update();
    enemyList->update();
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
    //objectList->renderT3d();
    repairableList->renderT3d();
    enemyList->renderT3d();
    
    //global::thePlayer->renderT3d();
    
    t3d_matrix_set(envMatFP, true);
    t3d_model_draw(envModel);
    barricadeList->renderT3d();
    theCursor->renderT3d();
    t3d_matrix_pop(1);
}

void GS_Level01::renderRdpq() {
    //objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();
    //global::thePlayer->renderRdpq();
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