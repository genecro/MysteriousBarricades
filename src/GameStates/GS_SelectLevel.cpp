#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_SelectLevel::GS_SelectLevel(T3DVec3 startingPos) {
    levelLockedSprite = sprite_load("rom:/sprites/lockedLevel.rgba32.sprite");
    envModel = nullptr;
}

GS_SelectLevel::~GS_SelectLevel() {
    sprite_free(levelLockedSprite);
}

void GS_SelectLevel::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int xAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);
    int yAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    switch(currSelection) {
        case SELECTION_TRAINING:
            if(yAxis < 0) {
                currSelection = SELECTION_LEVEL_1;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Training01>((T3DVec3){0,10,0}, 1200.0f));
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
            else if(btn.a) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level01>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_LEVEL_2:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
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
            else if(btn.a) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Level02>((T3DVec3){0,10,0}, 1200.0f));
            }
        break;
        case SELECTION_BOSS1:
            if(yAxis > 0) {
                currSelection = SELECTION_TRAINING;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(global::gameProgress.level2Unlocked && xAxis < 0) {
                currSelection = SELECTION_LEVEL_2;
                global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
            }
            else if(btn.a) {
                global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
                //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_Boss01>((T3DVec3){0,0,0}, 1200.0f));
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