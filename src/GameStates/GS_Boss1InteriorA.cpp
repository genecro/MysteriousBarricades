#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"


GS_Boss1InteriorA::GS_Boss1InteriorA(T3DVec3 startingPlayerPos) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);
    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    envModel = t3d_model_load("rom:/boss1interiorA.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/boss1interiorA.bin");

    envScale = 0.8f;
    collision::scaleTriangles(&collisionTris, envScale);
    envModel->aabbMax[0]*=envScale;
    envModel->aabbMin[0]*=envScale;
    envModel->aabbMax[2]*=envScale;
    envModel->aabbMin[2]*=envScale;

    t3d_viewport_set_projection(viewport, 75.0f, 20.0f, 200.0f);

    thePlayer_ = new GO_Player(global::playerName);

    thePlayer_->position_ = (T3DVec3){0,0,320} + (T3DVec3){{0, thePlayer_->objectWidth_, 0}};
    thePlayer_->rotation_ = -T3D_PI/2.0f;

    initCamera();

    objectList = new GameObjectList();

    GO_NPCKnight* knight1 = new GO_NPCKnight("Knight1", (T3DVec3){{0,0,0}}, T3D_PI/2.0f);
    knight1->setInteractFunction([&](){
        if(!global::gameProgress.boss1RewardReceived) {
            global::GameInterruptStack->push_back(
                    (new GI_Alert(global::playerName + "...", false))
                ->setNextInterrupt(
                    (new GI_Alert("I hear it no longer...", false))
                ->setNextInterrupt(
                    (new GI_Alert("That hulking colossus...", false))
                ->setNextInterrupt(
                    (new GI_Alert("Its rain of missiles...", false))
                ->setNextInterrupt(
                    (new GI_Alert("It has ceased!", false))
                ->setNextInterrupt(
                    (new GI_Alert("Our structures have been spared!", false))
                ->setNextInterrupt(
                    (new GI_Alert("You, master of impedimancy, have\nsaved our realm!", false))
                ->setNextInterrupt(
                    (new GI_Alert("We shall forever remember you\nand your...", false))
                ->setNextInterrupt(
                    (new GI_Alert("...Mysterious Barricades.", false))
                ->setNextInterrupt(
                    (new GI_Alert("...", false))
                ->setNextInterrupt(
                    (new GI_Alert("Oh, right! A messenger from the\nnorthern realm has informed us\nthey are under a similar attack...", false))
                ->setNextInterrupt(
                    (new GI_Alert("...but that will have to wait for\nanother installment.", false))
                ->setNextInterrupt(
                    (new GI_Alert("In the meantime, we have added\nanother floor to our Training\nTower.", false))
                ->setNextInterrupt(
                    (new GI_Action([&](){
                        global::gameProgress.challenge1Unlocked = true;
                        global::audioManager->playSFX("mechaLevelUp5.wav64", {.volume = 0.4f});
                    })
                )
                ->setNextInterrupt(
                    (new GI_Alert("Training Tower: Challenge\nFloor unlocked!", false, true))
                ->setNextInterrupt(
                    (new GI_Alert("We hope you may use it to further\nhone your skills of deflection.", false))
                ->setNextInterrupt(
                    (new GI_Alert("Once again, thank you. And know\nthat wherever you may journey,\nyou will always be welcome here.", false))
                )))))))))))))))));
            global::gameProgress.boss1RewardReceived = true;
        }
        else {
            global::GameInterruptStack->push_back(
                (new GI_Alert("One more thing:", false))
            ->setNextInterrupt(
                (new GI_Alert("Since saving has not been\nimplemented, if you want to unlock\neverything, press this sequence", false))
            ->setNextInterrupt(
                (new GI_Alert("of buttons on the Level\nSelect screen: c_up, c_right,\nc_down, c_left", false))
            )));
        }
    });
    objectList->push(knight1);
    GO_Grate* grate1 = new GO_Grate((T3DVec3){0,0,212}, 0.0f);
    GO_Grate* grate2 = new GO_Grate((T3DVec3){0,0,53}, 0.0f);
    objectList->push(grate1);
    objectList->push(grate2);
    GO_Cannon* cannon1 = new GO_Cannon((T3DVec3){-32, 5, 220}, 0.0f, 205.0f, 350.0f, (color_t){0xBB, 0xBB, 0xBB, 0xFF}, (color_t){0xFF, 0, 0, 0xFF});
    GO_Cannon* cannon2 = new GO_Cannon((T3DVec3){-40, 5, 74}, -T3D_PI/2.0f, 50.0f, 205.0f, (color_t){0xBB, 0xBB, 0xBB, 0xFF}, (color_t){0xFF, 0, 0, 0xFF});
    objectList->push(cannon1);
    objectList->push(cannon2);
    std::vector<GameObject*> gratesToActivate1 = {grate1, cannon1};
    std::vector<GameObject*> gratesToActivate2 = {grate2, cannon2};
    GO_Target* target1 = new GO_Target((T3DVec3){-40, 5, 250}, -T3D_PI/2.0f, gratesToActivate1);
    GO_Target* target2 = new GO_Target((T3DVec3){-40, 5, 105}, -T3D_PI/2.0f, gratesToActivate2);
    objectList->push(target1);
    objectList->push(target2);
    barricadeList = new BarricadeList();
    enemyList = new EnemyList(&collisionTris);
    global::audioManager->playBGM(BGM_TRAINING, 0.4f);
}

GS_Boss1InteriorA::~GS_Boss1InteriorA() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete barricadeList;
    delete enemyList;
    delete thePlayer_;
    t3d_viewport_destroy(&viewport);
}

void GS_Boss1InteriorA::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(keys.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Boss1InteriorA>());
    }

    if(keys.b) {
        
    }

    thePlayer_->handleInput();
    handleInputCamera();
    objectList->handleInput();
    barricadeList->handleInput();
}

void GS_Boss1InteriorA::update() {
    //if(thePlayer_->position_.z > 85) {
    //    nextStatePop = true;
    //}
    thePlayer_->update();
    updateCamera();

    if(thePlayer_->position_.z > 325) {
        nextStatePop = true;
    }

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

void GS_Boss1InteriorA::renderT3d() {
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

void GS_Boss1InteriorA::renderRdpq() {
    objectList->renderRdpq();
    barricadeList->renderRdpq();
    thePlayer_->renderRdpq();
}

void GS_Boss1InteriorA::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.pos = (T3DVec3){{-15, 10, 15}};
    camera.target = thePlayer_->position_;
}

void GS_Boss1InteriorA::handleInputCamera() {
    
}

void GS_Boss1InteriorA::updateCamera() {
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