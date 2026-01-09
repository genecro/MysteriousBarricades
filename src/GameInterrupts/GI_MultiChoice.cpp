#include "GI_MultiChoice.h"
#include "../globals.h"

#define SCREEN_PADDING 50
#define BOX_HEIGHT 75
#define LINE_HEIGHT 25

GI_MultiChoice::~GI_MultiChoice() {
    for(auto const& [key, value] : menuChoices) {
        if(value && key.compare(currentChoiceString_) != 0) delete value;
    }
}

void GI_MultiChoice::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxisPressed = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    if(yAxisPressed) {
        currentChoice_ -= yAxisPressed;
        if(currentChoice_<0) {
            currentChoice_ = 0;
        }
        else if(currentChoice_>=numChoices_) {
            currentChoice_ = numChoices_ - 1;
        }
        else {
            global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
        }
    }

    if(btn.a) {
        nextInterrupt_ = menuChoices[currentChoiceString_];
        global::audioManager->playSFX("rom:/zapSelect5.wav64", {.volume = 0.7f});
        timeToDestroy = true;
    }
}

void GI_MultiChoice::update() {

}

void GI_MultiChoice::renderT3d() {

}

void GI_MultiChoice::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));
    rdpq_fill_rectangle(SCREEN_PADDING, display_get_height()/2.0f - numChoices_/2.0f*LINE_HEIGHT, display_get_width()-SCREEN_PADDING, display_get_height()/2.0f + numChoices_/2.0f*LINE_HEIGHT);

    int i = 0;
    for(auto const& [key, value] : menuChoices) {
        rdpq_text_printf(&(rdpq_textparms_t) {
            .style_id= i==currentChoice_ ? FONTSTYLE_RED : FONTSTYLE_WHITE,
        }, FONT_PIACEVOLI_16, SCREEN_PADDING+20, display_get_height()/2.0f - numChoices_/2.0f*LINE_HEIGHT + i*LINE_HEIGHT + LINE_HEIGHT/1.6f, key.c_str());
        if(i==currentChoice_) currentChoiceString_ = key;
        i++;
    }
    
}