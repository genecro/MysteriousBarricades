#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <string>
#include "../GameInterrupts/GI_Pause.h"
#include "../GameInterrupts/GI_FadeToNextGS.h"
#include "../GameInterrupts/GI_FadeIn.h"
#include "GameState.h"
//#include "GS_Intro.h"
//#include "GS_001ChurchSquare.h"
#include "../Fonts.h"
//#include "../globals.h"


#define NUM_MENU_CHOICES 3
#define MOVE_CURSOR_UP -1
#define MOVE_CURSOR_DOWN 1
#define SCREEN_PADDING 25
#define NUM_CREDITS_PAGES 6

class GS_Menu : public GameState
{
public:
    GS_Menu();
    ~GS_Menu();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

private:
    /*struct camera_t {
        T3DVec3 pos;
        T3DVec3 target;
    } camera;*/

    ////t3d
    T3DViewport viewport;
    T3DMat4 modelMat;
    T3DMat4FP* modelMatFP;
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
    uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec;
    T3DModel *model;
    float rotAngle;
    bool rotAngleReverse;
    float zTrans;
    bool zTransUp;
    rspq_block_t *dplDraw = NULL;

    sprite_t *titleSprite;
    int titleSpriteX, titleSpriteY;

    struct menuChoice {
        char const* choiceName;
        bool enabled;
    };

    void updateMainMenuState();
    void updateNewGameState();
    void updateCreditsGameState();

    struct nameEntry_t {
        bool caps;
        int numCols;
        std::string newPlayerName;
        int nameCursor;
        int LETTERS_IN_ALPHABET;
        int capsConv;
        int maxNameLen;
    } nameEntry;

    menuChoice menuChoices[NUM_MENU_CHOICES];
    int menuCursor;

    int textX, textY;

    int moveCursor(int currCursor, int direction);

    int currState;

    void drawTitleLogo();
    void drawMainMenu();
    void drawCredits();
    uint8_t currCreditsPage = 0;
    std::string creditsStr_[NUM_CREDITS_PAGES] = {
        "Logo font: \"English Towne\"\n    by Dieter Steffmann\n\nSmall in-game text: \"Piacevoli\"\n    from fontenddev.com\n\nLarge in-game text: \"Owre Kynge\"\n    from fontenddev.com",
        "Title music:\n    \"Les Barricades Mysterieuses\"\n    by Francois Couperin\n    from his 6th Ordre (1717)\n\nBoss music:\n    Fuguette a 3 voix\n    by Arthur Dodement (ca. 1912)",
        "Some textures from Craig Smith's\n    Retro Texture Pack\n    little-martian.dev\n\nSmall icons:\n    RunninBlood's RPG icon pack\n    runninblood.itch.io",
        "Sound effects from Helton Yan's\n    Pixel Combat pack\n    heltonyan.itch.io",
        "Player character (wizard) model,\nNPC (knight) model,\nand model animations\n    by Quaternius (quaternius.com)",
        "Thanks to the N64 homebrew\nDiscord and to the creators and\ncontributors of libdragon, tiny3d,\nand fast64 for making N64\nhomebrew what it is today!"
    };

    void drawNameEntry();
};