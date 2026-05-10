#include "Fonts.h"

void Fonts::fonts_init() {
    rdpq_font_t* freepixel = rdpq_font_load("rom:/FreePixel.font64");
    rdpq_font_t* owrekynge = rdpq_font_load("rom:/OwreKynge.font64");
    rdpq_font_t* piacevoli = rdpq_font_load("rom:/Piacevoli.font64");
    
    rdpq_text_register_font(FONT_FREE_12, freepixel);
    rdpq_text_register_font(FONT_OWREKYNGE_20, owrekynge);
    rdpq_text_register_font(FONT_PIACEVOLI_16, piacevoli);

    rdpq_fontstyle_t whiteFont =
    {
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    };

    rdpq_fontstyle_t redFont =
    {
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    };

    rdpq_fontstyle_t blueFont =
    {
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    };

    rdpq_fontstyle_t greyFont =
    {
        .color = RGBA32(0x88, 0x88, 0x88, 0xFF),
    };

    rdpq_fontstyle_t blackFont =
    {
        .color = RGBA32(0x00, 0x00, 0x00, 0xFF),
    };

    rdpq_font_style(freepixel, FONTSTYLE_WHITE, &whiteFont);
    rdpq_font_style(freepixel, FONTSTYLE_RED, &redFont);
    rdpq_font_style(freepixel, FONTSTYLE_BLUE, &blueFont);
    rdpq_font_style(freepixel, FONTSTYLE_GREY, &greyFont);

    rdpq_font_style(piacevoli, FONTSTYLE_WHITE, &whiteFont);
    rdpq_font_style(piacevoli, FONTSTYLE_RED, &redFont);
    rdpq_font_style(piacevoli, FONTSTYLE_BLUE, &blueFont);
    rdpq_font_style(piacevoli, FONTSTYLE_GREY, &greyFont);
    rdpq_font_style(piacevoli, FONTSTYLE_BLACK, &blackFont);

    rdpq_font_style(owrekynge, FONTSTYLE_WHITE, &whiteFont);
    rdpq_font_style(owrekynge, FONTSTYLE_RED, &redFont);
    rdpq_font_style(owrekynge, FONTSTYLE_BLUE, &blueFont);
}