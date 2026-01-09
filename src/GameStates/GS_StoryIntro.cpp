#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_StoryIntro::GS_StoryIntro(T3DVec3 startingPos) {
    global::GameInterruptStack->push_back(new GI_FadeIn(600));
}

GS_StoryIntro::~GS_StoryIntro() {
}

void GS_StoryIntro::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    
    if(btn.r) {
        global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f));
    }
}

void GS_StoryIntro::update() {

}

void GS_StoryIntro::renderT3d() {
    t3d_screen_clear_color(RGBA32(0, 0, 0, 0xFF));
}

void GS_StoryIntro::renderRdpq() {

    
}