#pragma once

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <string>
#include <vector>

class NumberGlyphDrawer
{
    public:
        NumberGlyphDrawer();
        ~NumberGlyphDrawer();

        void drawGlyph(int num, float xUL, float yUL, color_t color, float scale);

    private:
        sprite_t *numLine;
        std::vector<sprite_t*> *numberSprites;

        uint8_t SPRITE_WIDTH, LINE_WIDTH;
};