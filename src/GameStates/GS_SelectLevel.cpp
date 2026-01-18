#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_SelectLevel::GS_SelectLevel(T3DVec3 startingPos) {
    levelLockedSprite = sprite_load("rom:/sprites/lockedLevel.rgba32.sprite");
    envModel = nullptr;
    global::GameInterruptStack->push_back(new GI_FadeIn(600));
}

GS_SelectLevel::~GS_SelectLevel() {
    sprite_free(levelLockedSprite);
}

void GS_SelectLevel::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int xAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);
    int yAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    if(btn.c_up) {
        global::gameProgress.training2Unlocked = !global::gameProgress.training2Unlocked;
    }
    else if(btn.c_right) {
        global::gameProgress.training3Unlocked = !global::gameProgress.training3Unlocked;
    }
    else if(btn.c_left) {
        global::gameProgress.level2Unlocked = !global::gameProgress.level2Unlocked;
    }
    else if(btn.c_down) {
        global::gameProgress.boss1Unlocked = !global::gameProgress.boss1Unlocked;
    }

    switch(currSelection) {
        case SELECTION_TRAINING:
            if(yAxis < 0) {
                currSelection = SELECTION_LEVEL_1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(global::gameProgress.training2Unlocked && xAxis > 0) {
                currSelection = SELECTION_TRAINING_2;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Training01>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_TRAINING_2:
            if(yAxis < 0) {
                currSelection = global::gameProgress.level2Unlocked ? SELECTION_LEVEL_2 : SELECTION_LEVEL_1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if (global::gameProgress.training3Unlocked && xAxis > 0) {
                currSelection = SELECTION_TRAINING_3;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if (xAxis < 0) {
                currSelection = SELECTION_TRAINING;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Training02>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_TRAINING_3:
            if(yAxis < 0) {
                currSelection = global::gameProgress.boss1Unlocked ? SELECTION_BOSS1 : global::gameProgress.level2Unlocked ? SELECTION_LEVEL_2 : SELECTION_LEVEL_1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(xAxis < 0) {
                currSelection = SELECTION_TRAINING_2;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Training03>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_LEVEL_1:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(global::gameProgress.level2Unlocked && xAxis > 0) {
                currSelection = SELECTION_LEVEL_2;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level01>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_LEVEL_2:
            if(yAxis > 0) {
                currSelection = global::gameProgress.training2Unlocked ? SELECTION_TRAINING_2 : SELECTION_TRAINING;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(global::gameProgress.boss1Unlocked && xAxis > 0) {
                currSelection = SELECTION_BOSS1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(xAxis < 0) {
                currSelection = SELECTION_LEVEL_1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level02>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_BOSS1:
            if(yAxis > 0) {
                currSelection = global::gameProgress.training3Unlocked ? SELECTION_TRAINING_3 : global::gameProgress.training2Unlocked ? SELECTION_TRAINING_2 : SELECTION_TRAINING;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(global::gameProgress.level2Unlocked && xAxis < 0) {
                currSelection = SELECTION_LEVEL_2;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a || btn.start) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Boss1>((T3DVec3){10,10,-10}, 1200.0f));
            }
        break;
    }
}

void GS_SelectLevel::update() {

}

void GS_SelectLevel::renderT3d() {
    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
}

void GS_SelectLevel::renderRdpq() {

    uint8_t selectionBoxX=0;
    uint8_t selectionBoxY=0;
    std::string levelString="";

    switch(currSelection) {
        case SELECTION_TRAINING:
            selectionBoxX = trainingBoxX;
            selectionBoxY = trainingBoxY;
            levelString = "Training Tower: Level 1";
        break;

        case SELECTION_TRAINING_2:
            selectionBoxX = training2BoxX;
            selectionBoxY = training2BoxY;
            levelString = "Training Tower: Level 2";
        break;

        case SELECTION_TRAINING_3:
            selectionBoxX = training3BoxX;
            selectionBoxY = training3BoxY;
            levelString = "Training Tower: Level 3";
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
    rdpq_fill_rectangle(training2BoxX+selectBoxBorder, training2BoxY+selectBoxBorder, training2BoxX+selectBoxSize-selectBoxBorder, training2BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(training3BoxX+selectBoxBorder, training3BoxY+selectBoxBorder, training3BoxX+selectBoxSize-selectBoxBorder, training3BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(level01BoxX+selectBoxBorder, level01BoxY+selectBoxBorder, level01BoxX+selectBoxSize-selectBoxBorder, level01BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(level02BoxX+selectBoxBorder, level02BoxY+selectBoxBorder, level02BoxX+selectBoxSize-selectBoxBorder, level02BoxY+selectBoxSize-selectBoxBorder);
    rdpq_fill_rectangle(boss1BoxX+selectBoxBorder, boss1BoxY+selectBoxBorder, boss1BoxX+selectBoxSize-selectBoxBorder, boss1BoxY+selectBoxSize-selectBoxBorder);

    

    //draw level labels
    std::string lvlLbl = "T1";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_BLACK,
    }, FONT_PIACEVOLI_16, trainingBoxX+10, trainingBoxY+20, lvlLbl.c_str());

    lvlLbl = "T2";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= global::gameProgress.training2Unlocked ? FONTSTYLE_BLACK : FONTSTYLE_GREY,
    }, FONT_PIACEVOLI_16, training2BoxX+10, training2BoxY+20, lvlLbl.c_str());

    lvlLbl = "T3";
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= global::gameProgress.training3Unlocked ? FONTSTYLE_BLACK : FONTSTYLE_GREY,
    }, FONT_PIACEVOLI_16, training3BoxX+10, training3BoxY+20, lvlLbl.c_str());
    
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

    if(!global::gameProgress.training2Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, training2BoxX, training2BoxY, {});
    }

    if(!global::gameProgress.training3Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, training3BoxX, training3BoxY, {});
    }

    if(!global::gameProgress.level2Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, level02BoxX, level02BoxY, {});
    }

    if(!global::gameProgress.boss1Unlocked) {
        rdpq_sprite_blit(levelLockedSprite, boss1BoxX, boss1BoxY, {});
    }
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