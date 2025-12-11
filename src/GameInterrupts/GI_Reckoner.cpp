#include "GI_Reckoner.h"
#include "../globals.h"

#define STICK_DEADZONE 8
#define TOP_MENU_NUM_ITEMS 3
#define SCREEN_UPPER_LEFT_X 120
#define SCREEN_UPPER_LEFT_Y 80
#define SCREEN_UPPER_RIGHT_X 260
#define INVENTORY_LINE_HEIGHT 20
#define INVENTORY_ICON_WIDTH 20
#define INVENTORY_NAME_TEXT_WIDTH 70
#define QTY_TEXT_BOX_WIDTH 60
#define QTY_GLYPH_SCALE 2
#define QTY_GLYPH_WIDTH 10*QTY_GLYPH_SCALE
#define INV_SLOTS_TO_DISPLAY 5

enum {
    TOP_MENU = -1,
    MAP_STATE = 0,
    INV_STATE = 1,
    WB_STATE = 2,
};

enum {
    ITM_MNU_ABOUT = 0,
    ITM_MNU_USE = 1,
    ITM_MNU_BACK = 2,
};

GI_Reckoner::GI_Reckoner() {
    pauseStr = "Paused";
    pauseInterrupt = true;

    bgSprite = sprite_load("rom:/reckonerBG.sprite");

    wbSprite = sprite_load("rom:/sprites/RKWB.sprite");
    invSprite = sprite_load("rom:/sprites/RKINV.sprite");
    mapSprite = sprite_load("rom:/sprites/RKMAP.sprite");

    wbSpriteDark = sprite_load("rom:/sprites/RKWBdark.sprite");
    invSpriteDark = sprite_load("rom:/sprites/RKINVdark.sprite");
    mapSpriteDark = sprite_load("rom:/sprites/RKMAPdark.sprite");

    currState = TOP_MENU;

    topMenu_t.index = 0;

    invMenu_t.inventoryIndex = 0;
    invMenu_t.slotsSkipped = 0;
    invMenu_t.displayingDescription = false;
    invMenu_t.itemMenuEnabled = false;

    glyphDrawer = new NumberGlyphDrawer();

    playerInv = global::thePlayer->inventory_.items;

    inventoryLookup = new Inventory();
}

GI_Reckoner::~GI_Reckoner() {
    sprite_free(bgSprite);
    sprite_free(wbSprite);
    sprite_free(invSprite);
    sprite_free(mapSprite);
    sprite_free(wbSpriteDark);
    sprite_free(invSpriteDark);
    sprite_free(mapSpriteDark);
    delete glyphDrawer;
    delete inventoryLookup;
}

void GI_Reckoner::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    int yAxisHeld = joypad_get_axis_held(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    switch (currState) {
        case TOP_MENU:
            if(yAxisPressed < 0 || keys.d_down) {
                if(topMenu_t.index+1 < TOP_MENU_NUM_ITEMS) {
                    topMenu_t.index++;
                }
            }

            if(yAxisPressed > 0 || keys.d_up) {
                if(topMenu_t.index > 0) {
                    topMenu_t.index--;
                }
            }

            if(keys.a) {
                currState = topMenu_t.index;
            }

            if(keys.b || keys.start) {
                timeToDestroy = true;
            }
            break;

        case MAP_STATE:
            if(keys.b) {
                currState = TOP_MENU;
            }

            if(keys.start) {
                timeToDestroy = true;
            }
            break;
        
        case INV_STATE:
            if(invMenu_t.displayingDescription) {
                if(keys.a || keys.b) {
                    invMenu_t.displayingDescription = false;
                }
            }
            else if (invMenu_t.itemMenuEnabled) {
                if(keys.a) {
                    switch(invMenu_t.itemMenuIndex) {
                        case ITM_MNU_ABOUT:
                            invMenu_t.displayingDescription = true;
                            break;

                        case ITM_MNU_USE:
                            inventoryLookup->getItem(invMenu_t.currItemId).onUse();
                            break;

                        case ITM_MNU_BACK:
                            invMenu_t.itemMenuEnabled = false;
                            invMenu_t.itemMenuIndex = 0;
                            invMenu_t.itemMenuHighlightIndex = 0;
                            break;
                    }
                }

                if(keys.b) {
                    invMenu_t.itemMenuEnabled = false;
                    invMenu_t.itemMenuIndex = 0;
                    invMenu_t.itemMenuHighlightIndex = 0;
                }

                if(yAxisPressed > 0 || keys.d_up) {
                    if(invMenu_t.itemMenuIndex > ITM_MNU_ABOUT) {
                        invMenu_t.itemMenuIndex--;
                        invMenu_t.itemMenuHighlightIndex--;
                        if(invMenu_t.itemMenuIndex == ITM_MNU_USE && !inventoryLookup->getItem(invMenu_t.currItemId).canUse) {
                            invMenu_t.itemMenuIndex--;
                        }
                    }
                }
                else if(yAxisPressed < 0 || keys.d_down) {
                    if(invMenu_t.itemMenuIndex < ITM_MNU_BACK) {
                        invMenu_t.itemMenuIndex++;
                        invMenu_t.itemMenuHighlightIndex++;
                        if(invMenu_t.itemMenuIndex == ITM_MNU_USE && !inventoryLookup->getItem(invMenu_t.currItemId).canUse) {
                            invMenu_t.itemMenuIndex++;
                        }
                    }
                }
            }
            else {
                if(keys.b) {
                    currState = TOP_MENU;
                }

                if(keys.start) {
                    timeToDestroy = true;
                }

                if(keys.a) {
                    invMenu_t.itemMenuEnabled = true;
                    invMenu_t.itemMenuIndex = 0;
                    invMenu_t.itemMenuHighlightIndex = 0;
                }

                if(yAxisPressed < 0 || keys.d_down) {
                    if(invMenu_t.inventoryIndex + invMenu_t.slotsSkipped < playerInv->size() - 1) {
                        if(invMenu_t.inventoryIndex == INV_SLOTS_TO_DISPLAY - 1) {
                            invMenu_t.slotsSkipped++;
                        }
                        else {
                            invMenu_t.inventoryIndex++;
                        }
                    }
                }
                else if(yAxisPressed > 0 || keys.d_up) {
                    if(invMenu_t.inventoryIndex + invMenu_t.slotsSkipped != 0){
                        if(invMenu_t.inventoryIndex == 0) {
                            invMenu_t.slotsSkipped--;
                        }
                        else {
                            invMenu_t.inventoryIndex--;
                        }
                    }
                }
            }
            break;
        
        case WB_STATE:
            if(keys.b) {
                currState = TOP_MENU;
            }

            if(keys.start) {
                timeToDestroy = true;
            }
            break;
    }
}

void GI_Reckoner::update() {

}

void GI_Reckoner::renderT3d() {

}

void GI_Reckoner::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);

    rdpq_sprite_blit(
        bgSprite, 
        display_get_width()/2.0f, 
        display_get_height()/2.0f, 
        &(rdpq_blitparms_t){
            .cx = bgSprite->width/2.0,
            .cy = bgSprite->height/2.0,
        }
    );

    rdpq_sprite_blit(
        topMenu_t.index == 0 ? mapSprite : mapSpriteDark, 
        display_get_width()/4.0f, 
        display_get_height()/3.0f, 
        &(rdpq_blitparms_t){
            .cx=8,
            .cy=8,
            .scale_x=2,
            .scale_y=2,
        }
    );

    rdpq_sprite_blit(
        topMenu_t.index == 1 ? invSprite : invSpriteDark, 
        display_get_width()/4.0f, 
        display_get_height()/2.0f, 
        &(rdpq_blitparms_t){
            .cx=8,
            .cy=8,
            .scale_x=2,
            .scale_y=2,
        }
    );

    rdpq_sprite_blit(
        topMenu_t.index == 2 ? wbSprite : wbSpriteDark, 
        display_get_width()/4.0f, 
        2.0f*display_get_height()/3.0f, 
        &(rdpq_blitparms_t){
            .cx=8,
            .cy=8,
            .scale_x=2,
            .scale_y=2,
        }
    );

    switch (currState) {
        case TOP_MENU:
            rdpq_text_printf(&(rdpq_textparms_t) {
                .style_id=FONTSTYLE_ORANGE,
            }, FONT_PIXELFRAKTUR_24, display_get_width()/2, display_get_height()/2, pauseStr);

            switch (topMenu_t.index) {
                case MAP_STATE:
                    rdpq_text_printf(&(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                    }, FONT_PIXELFRAKTUR_16, display_get_width()/4.0f+20, display_get_height()/3.0f+6, "Plot");
                    break;

                case INV_STATE:
                    rdpq_text_printf(&(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                    }, FONT_PIXELFRAKTUR_16, display_get_width()/4.0f+20, display_get_height()/2.0f+6, "Hoard");
                    break;

                case WB_STATE:
                    rdpq_text_printf(&(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                    }, FONT_PIXELFRAKTUR_16, display_get_width()/4.0f+20, 2.0f*display_get_height()/3.0f+6, "Wordbook");
                    break;
            }
            break;
        
        case INV_STATE:
            if(invMenu_t.displayingDescription || invMenu_t.itemMenuEnabled) {
                //rdpq_set_mode_standard();
                //rdpq_mode_alphacompare(1);
                //rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
                rdpq_set_mode_standard();
                rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
                rdpq_mode_alphacompare(1);
                rdpq_set_prim_color(RGBA32(0xF8, 0x7D, 0x36, 0xFF)); //temporary fix, item sprite appears white without this - why?

                //item icon
                rdpq_sprite_blit(
                    inventoryLookup->getItem(invMenu_t.currItemId).itemSprite, 
                    SCREEN_UPPER_LEFT_X, 
                    SCREEN_UPPER_LEFT_Y, 
                    &(rdpq_blitparms_t){
                        .cx = 8,
                        .cy = 8,
                    }
                );

                //item name
                rdpq_text_printf(
                    &(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                    }, FONT_PIXELFRAKTUR_16, 
                    SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                    SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * (0.25f), 
                    inventoryLookup->getItem(invMenu_t.currItemId).itemName.c_str()
                );

                //item qty
                rdpq_text_printf(
                    &(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                        .width = QTY_TEXT_BOX_WIDTH,
                        .align = ALIGN_RIGHT,
                    }, FONT_PIXELFRAKTUR_16, 
                    SCREEN_UPPER_RIGHT_X - QTY_GLYPH_WIDTH - QTY_TEXT_BOX_WIDTH, 
                    SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * (0.25f),
                    std::to_string(playerInv->at(invMenu_t.currItemId)).c_str()
                );

                //item qty glyph
                glyphDrawer->drawGlyph(
                    playerInv->at(invMenu_t.currItemId), 
                    SCREEN_UPPER_RIGHT_X - QTY_GLYPH_WIDTH/2.0f, 
                    SCREEN_UPPER_LEFT_Y - QTY_GLYPH_WIDTH/2.0f, 
                    RGBA32(0xF8, 0x7D, 0x36, 0xFF), 
                    QTY_GLYPH_SCALE
                );

                if(invMenu_t.displayingDescription) {
                    int nbytes = inventoryLookup->getItem(invMenu_t.currItemId).itemDesc.length();

                    rdpq_paragraph_t *desc = rdpq_paragraph_build(&(rdpq_textparms_t) {
                        .style_id=FONTSTYLE_ORANGE,
                        .width = SCREEN_UPPER_RIGHT_X - SCREEN_UPPER_LEFT_X,
                        .wrap = WRAP_WORD,
                    }, FONT_PIXELFRAKTUR_16, inventoryLookup->getItem(invMenu_t.currItemId).itemDesc.c_str(), &nbytes);

                    rdpq_paragraph_render(desc, SCREEN_UPPER_LEFT_X, SCREEN_UPPER_LEFT_Y + 2*INVENTORY_LINE_HEIGHT);
                    rdpq_paragraph_free(desc);
                }
                else if(invMenu_t.itemMenuEnabled) {
                    rdpq_set_mode_fill(RGBA32(0xF8, 0x7D, 0x36, 0xFF));

                    rdpq_fill_rectangle(
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                        SCREEN_UPPER_LEFT_Y + (invMenu_t.itemMenuHighlightIndex + 0.75) * INVENTORY_LINE_HEIGHT,
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH + INVENTORY_NAME_TEXT_WIDTH,
                        SCREEN_UPPER_LEFT_Y + (invMenu_t.itemMenuHighlightIndex + 1.75) * INVENTORY_LINE_HEIGHT
                    );

                    int i = 0;

                    rdpq_text_printf(
                        &(rdpq_textparms_t) {
                            .style_id= invMenu_t.itemMenuHighlightIndex == i ? FONTSTYLE_BLACK : FONTSTYLE_ORANGE,
                        }, FONT_PIXELFRAKTUR_16, 
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                        SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * (i + 1.5f), 
                        "About"
                    );

                    i++;

                    if(inventoryLookup->getItem(invMenu_t.currItemId).canUse) {
                        rdpq_text_printf(
                            &(rdpq_textparms_t) {
                                .style_id= invMenu_t.itemMenuHighlightIndex == i ? FONTSTYLE_BLACK : FONTSTYLE_ORANGE,
                            }, FONT_PIXELFRAKTUR_16, 
                            SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                            SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * (i + 1.5f), 
                            inventoryLookup->getItem(invMenu_t.currItemId).useString.c_str()
                        );
                        i++;
                    }

                    rdpq_text_printf(
                        &(rdpq_textparms_t) {
                            .style_id= invMenu_t.itemMenuHighlightIndex == i ? FONTSTYLE_BLACK : FONTSTYLE_ORANGE,
                        }, FONT_PIXELFRAKTUR_16, 
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                        SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * (i + 1.5f), 
                        "Back"
                    );
                }
            }
            else {
                if(playerInv->size() > 0) {
                    rdpq_set_mode_fill(RGBA32(0xF8, 0x7D, 0x36, 0xFF));

                    rdpq_fill_rectangle(
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                        SCREEN_UPPER_LEFT_Y + (invMenu_t.inventoryIndex - 0.5) * INVENTORY_LINE_HEIGHT,
                        SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH + INVENTORY_NAME_TEXT_WIDTH,
                        SCREEN_UPPER_LEFT_Y + (invMenu_t.inventoryIndex + 0.5) * INVENTORY_LINE_HEIGHT
                    );
                }

                int i = 0;

                rdpq_set_mode_standard();
                rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
                rdpq_mode_alphacompare(1);
                rdpq_set_prim_color(RGBA32(0xF8, 0x7D, 0x36, 0xFF)); //temporary fix, item sprite appears white without this - why?

                for(auto& [key, value] : *playerInv) {
                    if(i >= invMenu_t.slotsSkipped && i < invMenu_t.slotsSkipped + INV_SLOTS_TO_DISPLAY){
                        
                        if(invMenu_t.inventoryIndex == i-invMenu_t.slotsSkipped) {
                            invMenu_t.currItemId = key;
                        }

                        if(inventoryLookup->getItem(key).itemSprite) {
                            //item icon
                            rdpq_sprite_blit(
                                inventoryLookup->getItem(key).itemSprite, 
                                SCREEN_UPPER_LEFT_X, 
                                SCREEN_UPPER_LEFT_Y + (i - invMenu_t.slotsSkipped)*INVENTORY_LINE_HEIGHT, 
                                &(rdpq_blitparms_t){
                                    .cx = 8,
                                    .cy = 8,
                                }
                            );

                            //item name
                            rdpq_text_printf(
                                &(rdpq_textparms_t) {
                                    .style_id= (i - invMenu_t.slotsSkipped) == invMenu_t.inventoryIndex ? FONTSTYLE_BLACK : FONTSTYLE_ORANGE,
                                }, FONT_PIXELFRAKTUR_16, 
                                SCREEN_UPPER_LEFT_X + INVENTORY_ICON_WIDTH, 
                                SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * ((i - invMenu_t.slotsSkipped) + 0.25f), 
                                inventoryLookup->getItem(key).itemName.c_str()
                            );

                            //item qty
                            rdpq_text_printf(
                                &(rdpq_textparms_t) {
                                    .style_id=FONTSTYLE_ORANGE,
                                    .width = QTY_TEXT_BOX_WIDTH,
                                    .align = ALIGN_RIGHT,
                                }, FONT_PIXELFRAKTUR_16, 
                                SCREEN_UPPER_RIGHT_X - QTY_GLYPH_WIDTH - QTY_TEXT_BOX_WIDTH, 
                                SCREEN_UPPER_LEFT_Y + INVENTORY_LINE_HEIGHT * ((i - invMenu_t.slotsSkipped) + 0.25f),
                                std::to_string(value).c_str()
                            );
                            
                            //item qty glyph
                            glyphDrawer->drawGlyph(
                                value, 
                                SCREEN_UPPER_RIGHT_X - QTY_GLYPH_WIDTH/2.0f, 
                                SCREEN_UPPER_LEFT_Y + (i - invMenu_t.slotsSkipped)*INVENTORY_LINE_HEIGHT - QTY_GLYPH_WIDTH/2.0f, 
                                RGBA32(0xF8, 0x7D, 0x36, 0xFF), 
                                QTY_GLYPH_SCALE
                            );
                        }
                    }
                    i++;
                }
            }
            break;
    }
}