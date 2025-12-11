#include "GI_Pause.h"

GI_Pause::GI_Pause() {
    pauseStr = "Paused";
    pauseInterrupt = true;
}

GI_Pause::~GI_Pause() {

}

void GI_Pause::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(keys.start) {
        timeToDestroy = true;
    }
}

void GI_Pause::update() {

}

void GI_Pause::renderT3d() {

}

void GI_Pause::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));

    rdpq_fill_rectangle(0, 0, display_get_width(), display_get_height());

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_TANNENBERG_20, display_get_width()/2-20, display_get_height()/2, pauseStr);
}