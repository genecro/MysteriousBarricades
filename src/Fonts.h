#pragma once
#include <libdragon.h>

enum {
    FONT_FREE_12 = 1,
    FONT_TANNENBERG_20 = 2,
    FONT_PIXELFRAKTUR_24 = 3,
    FONT_OWREKYNGE_14 = 4,
    FONT_BLACKFOREST_14 = 5,
    FONT_LEIPZIG_14 = 6,
    FONT_PIXELFRAKTUR_16 = 7,
};

enum {
    FONTSTYLE_WHITE = 1,
    FONTSTYLE_RED = 2,
    FONTSTYLE_BLUE = 3,
    FONTSTYLE_GREY = 4,
    FONTSTYLE_ORANGE = 5,
    FONTSTYLE_BLACK = 6,
};

class Fonts
{
public:
    static void fonts_init();
};