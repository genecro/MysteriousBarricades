#pragma once

#include <libdragon.h>
#include "../Fonts.h"
#include "GameInterrupt.h"
#include "../Misc/NumberGlyphDrawer.h"
#include "../Misc/Inventory.h"
#include <map>
//#include "../globals.h"

class GI_Reckoner : public GameInterrupt
{
public:
    GI_Reckoner();
    virtual ~GI_Reckoner();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    char* pauseStr;

    sprite_t *bgSprite;
    sprite_t *wbSprite;
    sprite_t *mapSprite;
    sprite_t *invSprite;
    sprite_t *wbSpriteDark;
    sprite_t *mapSpriteDark;
    sprite_t *invSpriteDark;

    NumberGlyphDrawer *glyphDrawer;

    int currState;

    struct topMenu_s {
        int index;
    } topMenu_t;

    struct invMenu_s {
        int inventoryIndex;
        int slotsSkipped;
        int currItemId;
        bool itemMenuEnabled;
        bool displayingDescription;
        uint8_t itemMenuIndex;
        uint8_t itemMenuHighlightIndex;
    } invMenu_t;

    std::map<int, int>* playerInv;

    Inventory* inventoryLookup;
};