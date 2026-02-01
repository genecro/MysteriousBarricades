//#include "GS_Menu.h"
//#include "GS_001ChurchSquare.h"
#include "GS_Menu.h"
#include "../globals.h"
#include "GS_StoryIntro.h"

enum {
    MENU_ITEM_NEW_GAME = 0,
    MENU_ITEM_OPTIONS = 1,
    MENU_ITEM_CREDITS = 2,
    MENU_ITEM_MAIN_MENU = 3,
};

GS_Menu::GS_Menu() {
    currState = MENU_ITEM_MAIN_MENU;
    nameEntry.newPlayerName = "";
    nameEntry.LETTERS_IN_ALPHABET = 26;
    nameEntry.caps = true;
    nameEntry.nameCursor = 0;
    nameEntry.capsConv = -32;
    nameEntry.numCols = 7;
    nameEntry.maxNameLen = 10;

    pausable = false;

    viewport = t3d_viewport_create();
    t3d_mat4_identity(&modelMat);
    modelMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));

    camera.pos = {{0, 10.0f, 40.0f}};
    camera.target = {{0,0,0}};
    

    lightDirVec = {{-1.0f, 1.0f, 1.0f}};
    t3d_vec3_norm(&lightDirVec);

    model = t3d_model_load("rom:/mainMenuModel.t3dm");

    rotAngle = T3D_PI;
    rotAngleReverse = false;
    zTrans = -10.0f;
    zTransUp = false;

    titleSprite = sprite_load("rom:/sprites/mystbarLogo.rgba32.sprite");
    assertf(titleSprite, "missing title sprite");

    titleSpriteX = (display_get_width() - titleSprite->width) / 2;
    titleSpriteY = 20;

    //drawn = false;
    menuChoices[MENU_ITEM_NEW_GAME] = (menuChoice{"New Game", 1});
    menuChoices[MENU_ITEM_OPTIONS] = (menuChoice{"Load Game", 0});
    menuChoices[MENU_ITEM_CREDITS] = (menuChoice{"Credits", 1});

    menuCursor = 0;

    textX = display_get_width()/2 - 45;
    textY = display_get_height()/2 + 50;

    global::GameInterruptStack->push_back(new GI_FadeIn());
    global::audioManager->playBGM(BGM_MAIN_MENU, 0.8f);
}

GS_Menu::~GS_Menu() {
    //if(global::thePlayer) {
    //    delete global::thePlayer;
    //}
    sprite_free(titleSprite);
    t3d_model_free(model);
    rspq_block_free(dplDraw);
    free_uncached(modelMatFP);
    global::audioManager->clearSFX();
}

void GS_Menu::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    /*
    if(keys.start) {
        global::GameInterruptStack->push_back(new GI_Pause());
    }
    */

    switch(currState) {
        case MENU_ITEM_MAIN_MENU:
            updateMainMenuState();
            break;
        case MENU_ITEM_NEW_GAME:
            updateNewGameState();
            break;
        case MENU_ITEM_CREDITS:
            updateCreditsGameState();
            break;
    }
}

void GS_Menu::update() {
    rotAngle += 0.005f*(rotAngleReverse*2-1);
    if(rotAngle > 6+T3D_PI || rotAngle < -6+T3D_PI) {
        rotAngleReverse = !rotAngleReverse;
    }

    zTrans += 0.1f*(zTransUp*2 - 1);
    if (zTrans < -70 || zTrans > -10) {
        zTransUp = !zTransUp;
    }
    float modelScale = 0.1f;

    camera.target = (T3DVec3){0,zTrans+10*(-zTrans/20.0f),0};

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camera.pos, &camera.target, &(T3DVec3){{0,1,0}});

    t3d_mat4_from_srt_euler(&modelMat,
      (float[3]){modelScale, modelScale, modelScale},
      //(float[3]){0.0f, rotAngle*0.2f, rotAngle},
      (float[3]){0.0f, rotAngle*0.2f, 0.0f},
      (float[3]){0,zTrans,0}
    );
    t3d_mat4_to_fixed(modelMatFP, &modelMat);
}

void GS_Menu::updateMainMenuState() {
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    if(keys.d_down || yAxisPressed < 0) {
        menuCursor = moveCursor(menuCursor, MOVE_CURSOR_DOWN);
    }
    if(keys.d_up || yAxisPressed > 0) {
        menuCursor = moveCursor(menuCursor, MOVE_CURSOR_UP);
    }

    if(keys.a) {// || keys.start) {
        switch(menuCursor) {
            case MENU_ITEM_NEW_GAME:
                debugf("New Game selected\n");
                currState = MENU_ITEM_NEW_GAME;
                nameEntry.caps = true;
                nameEntry.newPlayerName = "";
                nameEntry.nameCursor = 0;
                global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
                break;
            case MENU_ITEM_OPTIONS:
                debugf("Options selected\n");
                break;
            case MENU_ITEM_CREDITS:
                debugf("Credits selected\n");
                currState = MENU_ITEM_CREDITS;
                global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
                break;
        }
    }

    //look around
    camera.target.y = inputs.stick_y;
    camera.target.x = inputs.stick_x;
}

void GS_Menu::updateNewGameState() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    int xAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);

    int nameCursorNext = nameEntry.nameCursor;

    if(keys.d_down || yAxisPressed < 0) {
        nameCursorNext += nameEntry.numCols;
        global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
    }
    else if(keys.d_up || yAxisPressed > 0) {
        nameCursorNext -= nameEntry.numCols;
        global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
    }
    else if(keys.d_left || xAxisPressed < 0) {
        nameCursorNext--;
        if(nameCursorNext%nameEntry.numCols==nameEntry.numCols-1) {
            nameCursorNext+=nameEntry.numCols;
        }
        global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
    }
    else if(keys.d_right || xAxisPressed > 0) {
        nameCursorNext++;
        if(nameCursorNext%nameEntry.numCols==0) {
            nameCursorNext-=nameEntry.numCols;
        }
        global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
    }

    if(nameCursorNext >= 0 && nameCursorNext < nameEntry.LETTERS_IN_ALPHABET) {
        nameEntry.nameCursor = nameCursorNext;
    }
    else if(nameCursorNext >= nameEntry.LETTERS_IN_ALPHABET) {
        nameEntry.nameCursor = nameEntry.LETTERS_IN_ALPHABET-1;
    }

    if(keys.r) {
        nameEntry.caps = !nameEntry.caps;
        global::audioManager->playSFX("rom:/strongClick26.wav64", {.volume = 0.7f});
    }

    if(keys.a && nameEntry.newPlayerName.size() < nameEntry.maxNameLen) {
        nameEntry.newPlayerName += ('a'+nameEntry.nameCursor+nameEntry.caps*nameEntry.capsConv);
        global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
    }

    if(keys.b && nameEntry.newPlayerName.size() > 0) {
        nameEntry.newPlayerName = nameEntry.newPlayerName.substr(0, nameEntry.newPlayerName.size()-1);
        global::audioManager->playSFX("rom:/zapSelect2.wav64", {.volume = 0.7f});
    }

    if(keys.l) {
        currState = MENU_ITEM_MAIN_MENU;
        global::audioManager->playSFX("rom:/zapSelect2.wav64", {.volume = 0.7f});
    }

    if(keys.start) {
        if(nameEntry.newPlayerName.size()>=3){
            //global::thePlayer = new GO_Player(nameEntry.newPlayerName);
            //TODO:
            //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_001ChurchSquare>(GS_001ChurchSquare::startPos.CHURCH_INTERIOR));//, 0.5*T3D_PI));
            //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_002ChurchInterior>((T3DVec3){{0,0,-150}}));
            global::playerName = nameEntry.newPlayerName;
            global::audioManager->playSFX("rom:/pierce6.wav64", {.volume = 0.7f});
            global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_StoryIntro>((T3DVec3){0,0,0}, 1200.0f));
        }
    }
}

void GS_Menu::updateCreditsGameState() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(keys.a) {
        currCreditsPage = (currCreditsPage + 1) % NUM_CREDITS_PAGES;
        global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
    }
    if(keys.b) {
        currState = MENU_ITEM_MAIN_MENU;
        global::audioManager->playSFX("rom:/zapSelect2.wav64", {.volume = 0.7f});
    }
}

void GS_Menu::renderT3d() {
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(0x87, 0xCE, 0xEB, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    
    
    if(!dplDraw) {
        rspq_block_begin();

        t3d_matrix_push(modelMatFP);
        // Draw the model, material settings (e.g. textures, color-combiner) are handled internally
        //rdpq_sync_load();
        //rdpq_sync_tile();
        t3d_model_draw(model);
        t3d_matrix_pop(1);
        dplDraw = rspq_block_end();
    }

    rspq_block_run(dplDraw);
}

void GS_Menu::renderRdpq() {
    switch (currState) {
        case MENU_ITEM_MAIN_MENU:
            drawTitleLogo();
            drawMainMenu();
            break;
        case MENU_ITEM_NEW_GAME:
            drawNameEntry();
            break;
        case MENU_ITEM_CREDITS:
            drawCredits();
            break;
    }
}

void GS_Menu::drawNameEntry() {
    char *enterName = "Enter your name:";

    int nameX = 40;
    int nameY = 40;

    int fontSize = 12;
    int alphaX = 40;
    int alphaY = 60;

    rdpq_sync_pipe();

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_BLUE,
    }, FONT_OWREKYNGE_20, nameX-10, nameY, enterName);
 
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_PIACEVOLI_16, nameX+145, nameY, (
        nameEntry.newPlayerName.size() < nameEntry.maxNameLen ? 
            nameEntry.newPlayerName+'_' : 
            nameEntry.newPlayerName).c_str());

    rdpq_sync_pipe();
    rdpq_set_mode_fill(RGBA32(0x66, 0x66, 0xFF, 0xFF));
    rdpq_sync_pipe();
    rdpq_fill_rectangle(alphaX-fontSize/2, alphaY-fontSize, alphaX + nameEntry.numCols*fontSize, alphaY + fontSize*nameEntry.LETTERS_IN_ALPHABET/nameEntry.numCols);
    rdpq_sync_pipe();
    for(int i = 0; i < nameEntry.LETTERS_IN_ALPHABET; i++) {
        int renderFont = i==nameEntry.nameCursor ? FONTSTYLE_RED : FONTSTYLE_WHITE;
        char currChar[] = {'a'+i+(nameEntry.caps*nameEntry.capsConv), '\0'};

        rdpq_text_printf(&(rdpq_textparms_t){
            .style_id=renderFont,
        }, FONT_PIACEVOLI_16, alphaX + (i%nameEntry.numCols)*fontSize, alphaY + (i/nameEntry.numCols)*fontSize, currChar);
    }

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_WHITE,
    }, FONT_PIACEVOLI_16, SCREEN_PADDING+10, display_get_height()/2.0f+10, "A: Select letter\nB: Backspace\nR: Toggle shift\nStart: Start game\nL: Back to main menu");
    rdpq_sync_pipe();
}

void GS_Menu::drawTitleLogo() {
    rdpq_sync_pipe();
    rdpq_sync_load();
    //rdpq_set_mode_copy(true);
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(titleSprite, titleSpriteX, titleSpriteY, NULL);
    if(nextState != nullptr) {
        sprite_free(titleSprite);
    }
}

void GS_Menu::drawMainMenu() {
    for(int i = 0; i < NUM_MENU_CHOICES; i++) {
        //debugf("Menu choice %d\n", i);
        int renderFont = FONTSTYLE_WHITE;
        if(i==menuCursor) {
            renderFont = FONTSTYLE_RED;
        }
        if(!menuChoices[i].enabled){
            renderFont = FONTSTYLE_GREY;
        }
        rdpq_text_printf(&(rdpq_textparms_t){
            .style_id=renderFont,
        }, FONT_PIACEVOLI_16, textX, textY + i*12, menuChoices[i].choiceName);
    }
}

void GS_Menu::drawCredits() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));
    rdpq_fill_rectangle(SCREEN_PADDING, SCREEN_PADDING, display_get_width()-SCREEN_PADDING, display_get_height()-SCREEN_PADDING);

    rdpq_text_printf(&(rdpq_textparms_t) {
            .style_id= FONTSTYLE_WHITE,
        }, 
        FONT_PIACEVOLI_16, 
        SCREEN_PADDING+20, 
        SCREEN_PADDING+20, 
        creditsStr_[currCreditsPage].c_str()
    );
}

int GS_Menu::moveCursor(int currCursor, int direction) {
    bool done = false;
    int nextCursor = currCursor;
    while (!done) {
        nextCursor+= direction;

        if (nextCursor <0 || nextCursor > NUM_MENU_CHOICES-1) {
            return currCursor;
        }
        else if (menuChoices[nextCursor].enabled) {
            done = true;
        }
    }
    global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
    return nextCursor;
}