#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"


GS_Training03InteriorA::GS_Training03InteriorA(T3DVec3 startingPlayerPos) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);
    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    envModel = t3d_model_load("rom:/training03interiorA.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/training03interiorA.bin");

    envScale = 0.8f;
    /*debugf("BEFORE SCALING***************\n");
    for (auto tri : collisionTris) {
        debugf("v0 x: %.2f, y: %.2f, z: %.2f\n", tri.v0.x, tri.v0.y, tri.v0.z);
        debugf("v1 x: %.2f, y: %.2f, z: %.2f\n", tri.v1.x, tri.v1.y, tri.v1.z);
        debugf("v2 x: %.2f, y: %.2f, z: %.2f\n\n", tri.v2.x, tri.v2.y, tri.v2.z);
    }*/
    collision::scaleTriangles(&collisionTris, envScale);
    /*debugf("AFTER SCALING****************\n");
    for (auto tri : collisionTris) {
        debugf("v0 x: %.2f, y: %.2f, z: %.2f\n", tri.v0.x, tri.v0.y, tri.v0.z);
        debugf("v1 x: %.2f, y: %.2f, z: %.2f\n", tri.v1.x, tri.v1.y, tri.v1.z);
        debugf("v2 x: %.2f, y: %.2f, z: %.2f\n\n", tri.v2.x, tri.v2.y, tri.v2.z);
    }*/

    t3d_viewport_set_projection(viewport, 75.0f, 20.0f, 200.0f);

    thePlayer_ = new GO_Player("name");

    thePlayer_->position_ = startingPlayerPos + (T3DVec3){{0, thePlayer_->objectWidth_, 0}};

    initCamera();

    objectList = new GameObjectList();
    barricadeList = new BarricadeList();
    enemyList = new EnemyList(&collisionTris);
}

GS_Training03InteriorA::~GS_Training03InteriorA() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete barricadeList;
    delete enemyList;
    delete thePlayer_;
}

void GS_Training03InteriorA::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(keys.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Training03InteriorA>());
    }

    if(keys.b) {
        
    }

    thePlayer_->handleInput();
    handleInputCamera();
    objectList->handleInput();
    barricadeList->handleInput();
}

void GS_Training03InteriorA::update() {
    thePlayer_->update();
    updateCamera();

    

    t3d_mat4_from_srt_euler(&envMat,
        (float[3]){ envScale, envScale, envScale},
        (float[3]){0.0f, 0.0f, 0.0f},
        (float[3]){0,0,0}
    );
    t3d_mat4_to_fixed(envMatFP, &envMat);

    objectList->update(thePlayer_->position_);
    barricadeList->update();
    enemyBarricadeCheck();
    projectileBarricadeCheck();
}

void GS_Training03InteriorA::renderT3d() {
    t3d_viewport_set_projection(viewport, camera.FOV, 10.0f, 200.0f);
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(160, 110, 200, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);

    t3d_viewport_look_at(viewport, camera.pos, camera.target, {{0, 1, 0}});

    t3d_matrix_push_pos(1);
    objectList->renderT3d();
    
    t3d_matrix_set(envMatFP, true);
    t3d_model_draw(envModel);
    thePlayer_->renderT3d();
    barricadeList->renderT3d();
    t3d_matrix_pop(1);
}

void GS_Training03InteriorA::renderRdpq() {
    objectList->renderRdpq();
    barricadeList->renderRdpq();
    thePlayer_->renderRdpq();
}

void GS_Training03InteriorA::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.pos = (T3DVec3){{-15, 10, 15}};
    camera.target = thePlayer_->position_;
}

void GS_Training03InteriorA::handleInputCamera() {
    
}

void GS_Training03InteriorA::updateCamera() {
    camera.target = thePlayer_->position_;
    camera.target += {{0,2,0}};
    // if(camera.target.z>9.0f) {
    //     camera.target.z = 9.0f;
    // }
    // if(camera.target.x<-8.5f) {
    //     camera.target.x = -8.5f;
    // }

    camera.pos = thePlayer_->position_+(T3DVec3){25, 25, 25};
    //camera.pos = thePlayer_->position_+(T3DVec3){1, 40, 1};
}