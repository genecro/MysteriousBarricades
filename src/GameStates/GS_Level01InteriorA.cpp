#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"


GS_Level01InteriorA::GS_Level01InteriorA(T3DVec3 startingPlayerPos) {
    viewport = t3d_viewport_create();
    t3d_mat4_identity(&envMat);
    envMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    envModel = t3d_model_load("rom:/level01interiorA.t3dm");
    collisionTris = collision::loadCollTriangles("rom:/level01interiorA.bin");

    envScale = 0.6f;
    collision::scaleTriangles(&collisionTris, envScale);

    t3d_viewport_set_projection(viewport, 75.0f, 20.0f, 200.0f);

    thePlayer_ = new GO_Player(global::playerName);

    thePlayer_->position_ = startingPlayerPos + (T3DVec3){{0, thePlayer_->objectWidth_, 0}};
    thePlayer_->rotation_ = -T3D_PI/2.0f;

    initCamera();

    objectList = new GameObjectList();

    GO_NPCKnight* knight1 = new GO_NPCKnight("Knight1", (T3DVec3){{0,0,0}}, T3D_PI/2.0f);
    knight1->setInteractFunction([&](){
        if(!global::gameProgress.level1RewardReceived) {
            global::GameInterruptStack->push_back(
                    (new GI_Alert("Stay back!!!", false))
                ->setNextInterrupt(
                    (new GI_Alert("Oh...", false))
                ->setNextInterrupt(
                    (new GI_Alert(global::playerName + "! It's you!", false))
                ->setNextInterrupt(
                    (new GI_Alert("I shouldn't be surprised. Only an\nImpedimancer of your caliber could\nhave made it through my defenses.", false))
                ->setNextInterrupt(
                    (new GI_Alert("Thank you for repairing my tower.\nPlease accept this gift.", false))
                ->setNextInterrupt(
                    (new GI_Action([&](){
                        global::gameProgress.barricadesCanRicochet = true;
                        global::audioManager->playSFX("mechaLevelUp5.wav64", {.volume = 0.4f});
                    })
                )
                ->setNextInterrupt(
                    (new GI_Alert("Barricades will now\nricochet projectiles", false, true))
                ->setNextInterrupt(
                    (new GI_Alert("Now any projectiles making\ncontact with your barricades will\nbounce off of them.", false))
                ->setNextInterrupt(
                    (new GI_Alert("You will certainly need this\nability for the ordeals which\nlie ahead.", false))
                ->setNextInterrupt(
                    (new GI_Alert("And you may even find some\nuse for it in the entrance\ncorridor of this tower...", false))
                ->setNextInterrupt(
                    (new GI_Alert("Good luck! We are forever\ngrateful for your services\nto our realm.", false))
                )))))))))));
            global::gameProgress.level1RewardReceived = true;
        }
        else {
            global::GameInterruptStack->push_back(new GI_Alert("Good luck! We are forever\ngrateful for your services\nto our realm.", false));
        }
    });
    objectList->push(knight1);

    barricadeList = new BarricadeList();
    enemyList = new EnemyList(&collisionTris);

    addProjectile.push_back({10,
        [&](){
            return true;
    }});

    addProjectile.push_back({5*60,
        [&](){
            T3DVec3 projectileOrigin = (T3DVec3){0,5,80};
            if(global::gameState->thePlayer_->position_.z <= 100.0f) {
                return false;
            }
            else {
                objectList->push(new GO_Projectile(projectileOrigin, -T3D_PI, 0.5f, nullptr, 30.0f));
                timeline = addProjectile;
            }
            return true;
    }});

    timeline = addProjectile;

    global::GameInterruptStack->push_back(new GI_FadeIn(600));
    //global::audioManager->playBGM(BGM_INTERIOR, 0.8f);
}

GS_Level01InteriorA::~GS_Level01InteriorA() {
    t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete barricadeList;
    delete enemyList;
    delete thePlayer_;
}

void GS_Level01InteriorA::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(keys.start) {
        global::GameInterruptStack->push_back(new GI_Pause<GS_Level01InteriorA>());
    }

    if(keys.b) {
        
    }

    thePlayer_->handleInput();
    handleInputCamera();
    objectList->handleInput();
    barricadeList->handleInput();
}

void GS_Level01InteriorA::update() {
    if(thePlayer_->position_.z > 325) {
        nextStatePop = true;
    }
    if(thePlayer_->collidedWithProjectile()) {
        global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level01InteriorA>((T3DVec3){0,0,320}, 600));
    }
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
    updateTimeline();
}

void GS_Level01InteriorA::renderT3d() {
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

void GS_Level01InteriorA::renderRdpq() {
    objectList->renderRdpq();
    barricadeList->renderRdpq();
    thePlayer_->renderRdpq();
}

void GS_Level01InteriorA::initCamera() {
    camera.FOV = 0.27*T3D_PI;
    camera.pos = (T3DVec3){{-15, 10, 15}};
    camera.target = thePlayer_->position_;
}

void GS_Level01InteriorA::handleInputCamera() {
    
}

void GS_Level01InteriorA::updateCamera() {
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