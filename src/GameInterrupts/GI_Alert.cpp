#include "GI_Alert.h"

#define SCREEN_PADDING 25
#define BOX_HEIGHT 50

GI_Alert::GI_Alert(int AlertID) {
    name_ = "???";
}

GI_Alert::GI_Alert(std::string name) {
    name_ = name;
}

GI_Alert::~GI_Alert() {

}

void GI_Alert::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.a) {
        timeToDestroy = true;
    }
}

void GI_Alert::update() {

}

void GI_Alert::renderT3d() {

}

void GI_Alert::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));
    rdpq_fill_rectangle(SCREEN_PADDING, display_get_height()-SCREEN_PADDING-BOX_HEIGHT, display_get_width()-SCREEN_PADDING, display_get_height()-SCREEN_PADDING);

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_TANNENBERG_20, SCREEN_PADDING+20, display_get_height()-SCREEN_PADDING-BOX_HEIGHT/2, name_.c_str());
}