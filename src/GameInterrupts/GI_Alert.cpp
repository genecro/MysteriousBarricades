#include <algorithm>
#include "GI_Alert.h"
#include "../globals.h"

#define SCREEN_PADDING 25
#define BOX_HEIGHT 75

GI_Alert::GI_Alert(std::string name, bool titleText = false) {
    titleText_ = titleText;
    name_ = name;

    numLines = std::count(name_.begin(), name_.end(), '\n') + 1;
}

GI_Alert::~GI_Alert() {
    // if(nextInterrupt_) {
    //     global::GameInterruptStack->push_back(nextInterrupt_);
    // }
}

void GI_Alert::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.a) {
        global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
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
            .style_id= titleText_ ? FONTSTYLE_RED : FONTSTYLE_WHITE,
        }, 
        titleText_ ? FONT_OWREKYNGE_20 : FONT_PIACEVOLI_16, 
        SCREEN_PADDING+20, 
        display_get_height()-SCREEN_PADDING-BOX_HEIGHT + (numLines <= 2 ? 30 : 20), 
        name_.c_str()
    );
}

// GI_Alert* GI_Alert::setNextAlert(GI_Alert* nextAlert) {
//     nextAlert_ = nextAlert;
//     return this;
// }