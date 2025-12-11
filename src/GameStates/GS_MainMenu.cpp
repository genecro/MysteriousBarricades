//#include "GS_MainMenu.h"
//#include "GS_001ChurchSquare.h"
#include "GS.h"
#include "../globals.h"

enum {
    MENU_ITEM_NEW_GAME = 0,
    MENU_ITEM_OPTIONS = 1,
    MENU_ITEM_CREDITS = 2,
    MENU_ITEM_MAIN_MENU = 3,
};

GS_MainMenu::GS_MainMenu() {
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

    model = t3d_model_load("rom:/chTwHTx.t3dm");

    rotAngle = 0.0f;
    rotAngleReverse = false;
    zTrans = -10.0f;
    zTransUp = false;

    titleSprite = sprite_load("rom:/sprites/titleLogo.sprite");
    assertf(titleSprite, "missing title sprite");

    titleSpriteX = (display_get_width() - titleSprite->width) / 2;
    titleSpriteY = 40;

    //drawn = false;
    menuChoices[MENU_ITEM_NEW_GAME] = (menuChoice{"New Game", 1});
    menuChoices[MENU_ITEM_OPTIONS] = (menuChoice{"Options", 1});
    menuChoices[MENU_ITEM_CREDITS] = (menuChoice{"Credits", 1});

    menuCursor = 0;

    textX = display_get_width()/2 - 45;
    textY = display_get_height()/2 + 50;

    global::GameInterruptStack->push_back(new GI_FadeIn());
}

GS_MainMenu::~GS_MainMenu() {
    //if(global::thePlayer) {
    //    delete global::thePlayer;
    //}
    sprite_free(titleSprite);
    t3d_model_free(model);
    rspq_block_free(dplDraw);
    free_uncached(modelMatFP);
}

void GS_MainMenu::handleInput() {
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
    }
}

void GS_MainMenu::update() {
    rotAngle += 0.005f*(rotAngleReverse*2-1);
    if(rotAngle > 6 || rotAngle < -6) {
        rotAngleReverse = !rotAngleReverse;
    }

    zTrans += 0.1f*(zTransUp*2 - 1);
    if (zTrans < -75 || zTrans > -1) {
        zTransUp = !zTransUp;
    }
    float modelScale = 0.1f;

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

void GS_MainMenu::updateMainMenuState() {
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    //int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    if(keys.d_down){// || yAxisPressed < 0) {
        menuCursor = moveCursor(menuCursor, MOVE_CURSOR_DOWN);
    }
    if(keys.d_up){// || yAxisPressed > 0) {
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
                break;
            case MENU_ITEM_OPTIONS:
                debugf("Options selected\n");
                break;
            case MENU_ITEM_CREDITS:
                debugf("Credits selected\n");
                break;
        }
    }

    //look around
    camera.target.y = inputs.stick_y;
    camera.target.x = inputs.stick_x;
}

void GS_MainMenu::updateNewGameState() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    int xAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);

    int nameCursorNext = nameEntry.nameCursor;

    if(keys.d_down || yAxisPressed < 0) {
        nameCursorNext += nameEntry.numCols;
    }
    else if(keys.d_up || yAxisPressed > 0) {
        nameCursorNext -= nameEntry.numCols;
    }
    else if(keys.d_left || xAxisPressed < 0) {
        nameCursorNext--;
        if(nameCursorNext%nameEntry.numCols==nameEntry.numCols-1) {
            nameCursorNext+=nameEntry.numCols;
        }
    }
    else if(keys.d_right || xAxisPressed > 0) {
        nameCursorNext++;
        if(nameCursorNext%nameEntry.numCols==0) {
            nameCursorNext-=nameEntry.numCols;
        }
    }

    if(nameCursorNext >= 0 && nameCursorNext < nameEntry.LETTERS_IN_ALPHABET) {
        nameEntry.nameCursor = nameCursorNext;
    }
    else if(nameCursorNext >= nameEntry.LETTERS_IN_ALPHABET) {
        nameEntry.nameCursor = nameEntry.LETTERS_IN_ALPHABET-1;
    }

    if(keys.r) {
        nameEntry.caps = !nameEntry.caps;
    }

    if(keys.a && nameEntry.newPlayerName.size() < nameEntry.maxNameLen) {
        nameEntry.newPlayerName += ('a'+nameEntry.nameCursor+nameEntry.caps*nameEntry.capsConv);
    }

    if(keys.b && nameEntry.newPlayerName.size() > 0) {
        nameEntry.newPlayerName = nameEntry.newPlayerName.substr(0, nameEntry.newPlayerName.size()-1);
    }

    if(keys.l) {
        currState = MENU_ITEM_MAIN_MENU;
    }

    if(keys.start) {
        if(nameEntry.newPlayerName.size()>=3){
            global::thePlayer = new GO_Player(nameEntry.newPlayerName);
            //TODO:
            //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_001ChurchSquare>(GS_001ChurchSquare::startPos.CHURCH_INTERIOR));//, 0.5*T3D_PI));
            //global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_002ChurchInterior>((T3DVec3){{0,0,-150}}));
        }
    }
}

void GS_MainMenu::renderT3d() {
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
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

void GS_MainMenu::renderRdpq() {
    switch (currState) {
        case MENU_ITEM_MAIN_MENU:
            drawTitleLogo();
            drawMainMenu();
            break;
        case MENU_ITEM_NEW_GAME:
            drawNameEntry();
            break;
    }
}

void GS_MainMenu::drawNameEntry() {
    char *enterName = "Enter your name:";

    int nameX = 40;
    int nameY = 40;

    int fontSize = 12;
    int alphaX = 40;
    int alphaY = 60;

    rdpq_sync_pipe();

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_BLUE,
    }, FONT_TANNENBERG_20, nameX, nameY, enterName);
 
    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_TANNENBERG_20, nameX+140, nameY, (
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
        }, FONT_FREE_12, alphaX + (i%nameEntry.numCols)*fontSize, alphaY + (i/nameEntry.numCols)*fontSize, currChar);
    }
    rdpq_sync_pipe();
}

void GS_MainMenu::drawTitleLogo() {
    rdpq_sync_pipe();
    rdpq_sync_load();
    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(titleSprite, titleSpriteX, titleSpriteY, NULL);
    if(nextState != nullptr) {
        sprite_free(titleSprite);
    }
}

void GS_MainMenu::drawMainMenu() {
    for(int i = 0; i < NUM_MENU_CHOICES; i++) {
        int renderFont = FONT_FREE_12;
        if(i==menuCursor) {
            renderFont = FONTSTYLE_RED;
        }
        if(!menuChoices[i].enabled){
            renderFont = FONTSTYLE_GREY;
        }
        rdpq_text_printf(&(rdpq_textparms_t){
            .style_id=renderFont,
        }, FONT_FREE_12, textX, textY + i*12, menuChoices[i].choiceName);
    }
}

int GS_MainMenu::moveCursor(int currCursor, int direction) {
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
    return nextCursor;
}

void GS_MainMenu::testFunc() { 
    debugf("Test func main menu\n");
}