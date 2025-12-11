#include "NumberGlyphDrawer.h"

NumberGlyphDrawer::NumberGlyphDrawer() {
    numLine = sprite_load("rom:/sprites/I4/numLine.sprite");

    numberSprites = new std::vector<sprite_t*>();

    for(int i = 0; i < 10; i++) {
        numberSprites->push_back(sprite_load(("rom:/sprites/I4/num0" + std::to_string(i) + ".sprite").c_str()));
    }

    SPRITE_WIDTH = numberSprites->at(0)->width;
    LINE_WIDTH = 1;
}

NumberGlyphDrawer::~NumberGlyphDrawer() {
    sprite_free(numLine);
    for(int i = 0; i < numberSprites->size(); i++) {
        sprite_free(numberSprites->at(i));
    }
    delete numberSprites;
}

void NumberGlyphDrawer::drawGlyph(int num, float xUL, float yUL, color_t color, float scale) {
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
    
    rdpq_mode_alphacompare(1);
    rdpq_set_prim_color(color);
    

    rdpq_sprite_blit(numLine, xUL, yUL, 
        &(rdpq_blitparms_t) {
            .scale_x = scale,
            .scale_y = scale,
        });

    //draw ones place
    rdpq_sprite_blit(numberSprites->at(num%10), xUL + scale*(SPRITE_WIDTH+LINE_WIDTH), yUL,
        &(rdpq_blitparms_t) {
            .scale_x = scale,
            .scale_y = scale,
        });

    //draw tens place
    rdpq_sprite_blit(numberSprites->at((num/10)%10), xUL, yUL, 
        &(rdpq_blitparms_t) {
            .flip_x = true,
            .scale_x = scale,
            .scale_y = scale,
        });

    //draw hundreds place
    rdpq_sprite_blit(numberSprites->at((num/100)%10), xUL + scale*(SPRITE_WIDTH+LINE_WIDTH),
                                                     yUL + scale*(SPRITE_WIDTH+LINE_WIDTH),
        &(rdpq_blitparms_t) {
            .flip_y = true,
            .scale_x = scale,
            .scale_y = scale,
        });

    //draw thousands place
    rdpq_sprite_blit(numberSprites->at((num/1000)%10), xUL, yUL + scale*(SPRITE_WIDTH+LINE_WIDTH),
        &(rdpq_blitparms_t) {
            .flip_x = true,
            .flip_y = true,
            .scale_x = scale,
            .scale_y = scale,
        });
}