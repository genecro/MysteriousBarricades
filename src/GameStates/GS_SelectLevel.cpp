#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_SelectLevel::GS_SelectLevel(T3DVec3 startingPos) {
    //viewport = t3d_viewport_create();

    levelLockedSprite = sprite_load("rom:/sprites/lockedLevel.rgba32.sprite");
    
    /*
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
    */
}

GS_SelectLevel::~GS_SelectLevel() {
    /*t3d_model_free(envModel);
    free_uncached(envMatFP);
    delete objectList;
    delete repairableList;
    delete barricadeList;
    delete enemyList;
    delete theCursor;*/
    sprite_free(levelLockedSprite);
}

void GS_SelectLevel::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int xAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);
    int yAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    //if(keys.start) {
    /*if(btn.start) {
        global::GameInterruptStack->push_back(new GI_Pause());
    }*/

    switch(currSelection) {
        case SELECTION_TRAINING:
            if(yAxis < 0) {
                currSelection = SELECTION_LEVEL_1;
            }
            else if(btn.a) {
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Training01>((T3DVec3){0,10,0}));
            }
        break;
        case SELECTION_LEVEL_1:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
            }
            else if(global::gameProgress.level2Unlocked && xAxis > 0) {
                currSelection = SELECTION_LEVEL_2;
            }
            else if(btn.a) {
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level01>((T3DVec3){0,10,0}));
            }
        break;
        case SELECTION_LEVEL_2:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
            }
            else if(global::gameProgress.boss1Unlocked && xAxis > 0) {
                currSelection = SELECTION_BOSS1;
            }
            else if(xAxis < 0) {
                currSelection = SELECTION_LEVEL_1;
            }
            else if(btn.a) {
                //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level02>((T3DVec3){0,10,0}));
            }
        break;
        case SELECTION_BOSS1:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
            }
            else if(global::gameProgress.level2Unlocked && xAxis < 0) {
                currSelection = SELECTION_LEVEL_2;
            }
            else if(btn.a) {
                //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Boss01>((T3DVec3){0,0,0}));
            }
        break;
    }

    /*
    theCursor->handleInput();

    handleInputCamera();
    objectList->handleInput();
    repairableList->handleInput();
    barricadeList->handleInput();
    enemyList->handleInput();
    */
}

void GS_SelectLevel::update() {
    /*
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
    */
}

void GS_SelectLevel::renderT3d() {
    /*
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
    */

    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
}

void GS_SelectLevel::renderRdpq() {
    /*objectList->renderRdpq();
    repairableList->renderRdpq();
    barricadeList->renderRdpq();
    enemyList->renderRdpq();*/

    uint8_t selectionBoxX=0;
    uint8_t selectionBoxY=0;
    std::string levelString="";

    switch(currSelection) {
        case SELECTION_TRAINING:
            selectionBoxX = trainingBoxX;
            selectionBoxY = trainingBoxY;
            levelString = "Training Tower (start here!)";
        break;

        case SELECTION_LEVEL_1:
            selectionBoxX = level01BoxX;
            selectionBoxY = level01BoxY;
            levelString = "Level 1";
        break;

        case SELECTION_LEVEL_2:
            selectionBoxX = level02BoxX;
            selectionBoxY = level02BoxY;
            levelString = "Level 2";
        break;

        case SELECTION_BOSS1:
            selectionBoxX = boss1BoxX;
            selectionBoxY = boss1BoxY;
            levelString = "Boss 1";
        break;
    }

    //draw red selection indicator
    rdpq_set_mode_fill((color_t){0xFF,0x77,0,0xFF});
    rdpq_fill_rectangle(selectionBoxX, selectionBoxY, selectionBoxX+selectBoxSize, selectionBoxY+selectBoxSize);

    //draw level boxes
    rdpq_set_mode_fill((color_t){0xFF,0xFF,0xFF,0xFF});
    rdpq_fill_rectangle(trainingBoxX+selectBoxBorder, trainingBoxY+selectBoxBorder, trainingBoxX+selectBoxSize-selectBoxBorder, trainingBoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(level01BoxX+selectBoxBorder, level01BoxY+selectBoxBorder, level01BoxX+selectBoxSize-selectBoxBorder, level01BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(level02BoxX+selectBoxBorder, level02BoxY+selectBoxBorder, level02BoxX+selectBoxSize-selectBoxBorder, level02BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(boss1BoxX+selectBoxBorder, boss1BoxY+selectBoxBorder, boss1BoxX+selectBoxSize-selectBoxBorder, boss1BoxY+selectBoxSize-selectBoxBorder);

    

    //draw level labels
    std::string lvlLbl = "T";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_BLACK,
    }, FONT_PIACEVOLI_16, trainingBoxX+10, trainingBoxY+20, lvlLbl.c_str());
    
    lvlLbl = "L1";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_BLACK,
    }, FONT_PIACEVOLI_16, level01BoxX+10, level01BoxY+20, lvlLbl.c_str());
    
    lvlLbl = "L2";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= global::gameProgress.level2Unlocked ? FONTSTYLE_BLACK : FONTSTYLE_GREY,
    }, FONT_PIACEVOLI_16, level02BoxX+10, level02BoxY+20, lvlLbl.c_str());

    lvlLbl = "B1";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= global::gameProgress.boss1Unlocked ? FONTSTYLE_BLACK : FONTSTYLE_GREY,
    }, FONT_PIACEVOLI_16, boss1BoxX+10, boss1BoxY+20, lvlLbl.c_str());

    std::string lvlSel = "Select Level";

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_OWREKYNGE_20, (float)display_get_width()/4.0f, (float)display_get_height()/4.0f, lvlSel.c_str());

    //draw level caption
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_WHITE,
    }, FONT_PIACEVOLI_16, (float)display_get_width()/4.0f, 3.0f*(float)display_get_height()/4.0f, levelString.c_str());

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);

    if(!global::gameProgress.level2Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, level02BoxX, level02BoxY, {});
    }

    if(!global::gameProgress.boss1Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, boss1BoxX, boss1BoxY, {});
    }
}

void GS_SelectLevel::testFunc() {
    debugf("Test func Training04\n");
}


void GS_SelectLevel::initCamera() {
    
}

void GS_SelectLevel::handleInputCamera() {
    
}

void GS_SelectLevel::updateCamera() {
    
}

void GS_SelectLevel::levelWon() {
    
}

void GS_SelectLevel::levelLost() {
    
}

void GS_SelectLevel::checkForWinOrLoss() {
    
}

void GS_SelectLevel::barricadeCreated() {
    
}