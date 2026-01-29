#include "GS.h"
#include "../GameObjects/GO.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"
#include "../collision.h"

GS_StoryIntro::GS_StoryIntro(T3DVec3 startingPos) {
    global::GameInterruptStack->push_back((new GI_Alert(storyPages[0], false))->setNextInterrupt(
        (new GI_Alert(storyPages[1], false))->setNextInterrupt(
        (new GI_Alert(storyPages[2], false))->setNextInterrupt(
        (new GI_Alert(storyPages[3], false))->setNextInterrupt(
        (new GI_Alert(storyPages[4], false))->setNextInterrupt(
        (new GI_Alert(storyPages[5], false))->setNextInterrupt(
        (new GI_Alert(storyPages[6], false))->setNextInterrupt(
        (new GI_Alert(storyPages[7], false))->setNextInterrupt(
        (new GI_Alert(storyPages[8], false))->setNextInterrupt(
        new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,0,0}, 600.0f)
        )))))))))
    );
    global::GameInterruptStack->push_back(new GI_FadeIn(600));
    global::audioManager->playBGM(BGM_INTRO, 0.8f);
}

GS_StoryIntro::~GS_StoryIntro() {
    global::audioManager->clearSFX();
}

void GS_StoryIntro::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    
    if(btn.r || btn.start) {
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