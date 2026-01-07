#include "Fonts.h"

void Fonts::fonts_init() {
    rdpq_font_t* freepixel = rdpq_font_load("rom:/FreePixel.font64");
    rdpq_font_t* pixelfraktur = rdpq_font_load("rom:/PixelFraktur.font64");
    rdpq_font_t* pixelfraktur16 = rdpq_font_load("rom:/PixelFraktur16.font64");
    rdpq_font_t* tannenberg = rdpq_font_load("rom:/Tannenberg.font64");
    rdpq_font_t* owrekynge = rdpq_font_load("rom:/OwreKynge.font64");
    rdpq_font_t* blackforest = rdpq_font_load("rom:/BlackForest.font64");
    rdpq_font_t* leipzig = rdpq_font_load("rom:/Leipzig.font64");
    rdpq_font_t* piacevoli = rdpq_font_load("rom:/Piacevoli.font64");
    
    rdpq_text_register_font(FONT_FREE_12, freepixel);
    rdpq_text_register_font(FONT_TANNENBERG_20, tannenberg);
    rdpq_text_register_font(FONT_PIXELFRAKTUR_24, pixelfraktur);
    rdpq_text_register_font(FONT_PIXELFRAKTUR_16, pixelfraktur16);
    rdpq_text_register_font(FONT_OWREKYNGE_20, owrekynge);
    rdpq_text_register_font(FONT_BLACKFOREST_14, blackforest);
    rdpq_text_register_font(FONT_LEIPZIG_14, leipzig);
    rdpq_text_register_font(FONT_PIACEVOLI_16, piacevoli);

    rdpq_font_style(freepixel, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
    rdpq_font_style(freepixel, FONTSTYLE_RED, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    });
    rdpq_font_style(freepixel, FONTSTYLE_BLUE, &(rdpq_fontstyle_t){
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    });
    rdpq_font_style(freepixel, FONTSTYLE_GREY, &(rdpq_fontstyle_t){
        .color = RGBA32(0x88, 0x88, 0x88, 0xFF),
    });

    rdpq_font_style(piacevoli, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
    rdpq_font_style(piacevoli, FONTSTYLE_RED, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    });
    rdpq_font_style(piacevoli, FONTSTYLE_BLUE, &(rdpq_fontstyle_t){
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    });
    rdpq_font_style(piacevoli, FONTSTYLE_GREY, &(rdpq_fontstyle_t){
        .color = RGBA32(0x88, 0x88, 0x88, 0xFF),
    });
    rdpq_font_style(piacevoli, FONTSTYLE_BLACK, &(rdpq_fontstyle_t){
        .color = RGBA32(0x00, 0x00, 0x00, 0xFF),
    });


    rdpq_font_style(pixelfraktur, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
    rdpq_font_style(pixelfraktur, FONTSTYLE_RED, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    });
    rdpq_font_style(pixelfraktur, FONTSTYLE_ORANGE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xF8, 0x7D, 0x36, 0xFF),
    });
    rdpq_font_style(pixelfraktur, FONTSTYLE_BLUE, &(rdpq_fontstyle_t){
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    });

    rdpq_font_style(pixelfraktur16, FONTSTYLE_ORANGE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xF8, 0x7D, 0x36, 0xFF),
    });

    rdpq_font_style(pixelfraktur16, FONTSTYLE_BLACK, &(rdpq_fontstyle_t){
        .color = RGBA32(0x01, 0x01, 0x01, 0xFF),
    });

    rdpq_font_style(tannenberg, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
    rdpq_font_style(tannenberg, FONTSTYLE_RED, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    });
    rdpq_font_style(tannenberg, FONTSTYLE_BLUE, &(rdpq_fontstyle_t){
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    });


    rdpq_font_style(owrekynge, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
    rdpq_font_style(owrekynge, FONTSTYLE_RED, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0x00, 0x00, 0xFF),
    });
    rdpq_font_style(owrekynge, FONTSTYLE_BLUE, &(rdpq_fontstyle_t){
        .color = RGBA32(0x66, 0x66, 0xFF, 0xFF),
    });

    rdpq_font_style(blackforest, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });

    rdpq_font_style(leipzig, FONTSTYLE_WHITE, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });
}