#pragma once
#include <libdragon.h>
#include "../Fonts.h"
#include "GameInterrupt.h"
#include "GI_FadeToNextGS.h"
#include "../globals.h"
#include "../GameStates/GS_SelectLevel.h"

template <typename T> class GI_Pause : public GameInterrupt
{
public:
    GI_Pause();
    ~GI_Pause();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    char* pauseStr;
    char* contStr;
    char* retryStr;
    char* quitStr;

    enum {
        PAUSE_SELECTION_CONT = 0,
        PAUSE_SELECTION_RETRY = 1,
        PAUSE_SELECTION_QUIT = 2
    };

    uint8_t currSelection = 0;
    uint8_t totalSelections = 3;
};

template <typename T> GI_Pause<T>::GI_Pause() {
    global::audioManager->playSFX("rom:/castingBuff2.wav64", {.volume = 0.5f});
    pauseStr = "Paused";
    contStr = "Continue";
    retryStr = "Retry";
    quitStr = "Quit";
    pauseInterrupt = true;
}

template <typename T> GI_Pause<T>::~GI_Pause() {

}

template <typename T> void GI_Pause<T>::handleInput() {
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    int yAxis = joypad_get_axis_pressed(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);

    if(keys.start) {
        timeToDestroy = true;
    }

    else if(yAxis) {
        currSelection-=yAxis;
        if(currSelection < 0) {
            currSelection = 0;
        }
        else if (currSelection >= totalSelections) {
            currSelection = totalSelections - 1;
        }
        else {
            global::audioManager->playSFX("rom:/bootsOnGenericGround6.wav64", {.volume = 0.4f});
        }
        //currSelection %= totalSelections;
    }

    if(keys.a) {
        global::audioManager->playSFX("rom:/zapSelect2.wav64", {.volume = 0.7f});
        switch(currSelection) {
            case PAUSE_SELECTION_CONT:
                timeToDestroy = true;
            break;

            case PAUSE_SELECTION_RETRY:
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<T>((T3DVec3){0,10,0}, 600.0f));
                timeToDestroy = true;
            break;

            case PAUSE_SELECTION_QUIT:
                global::GameInterruptStack->push_back(new GI_FadeToNextGS<GS_SelectLevel>((T3DVec3){0,10,0}, 600.0f));
                timeToDestroy = true;
            break;
        }
    }
}

template <typename T> void GI_Pause<T>::update() {

}

template <typename T> void GI_Pause<T>::renderT3d() {

}

template <typename T> void GI_Pause<T>::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));

    rdpq_fill_rectangle(0, 0, display_get_width(), display_get_height());

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_OWREKYNGE_20, display_get_width()/2-20, display_get_height()/2-40, pauseStr);

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= currSelection==PAUSE_SELECTION_CONT ? FONTSTYLE_RED : FONTSTYLE_WHITE,
    }, FONT_PIACEVOLI_16, display_get_width()/2-20, display_get_height()/2-10, contStr);

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= currSelection==PAUSE_SELECTION_RETRY ? FONTSTYLE_RED : FONTSTYLE_WHITE,
    }, FONT_PIACEVOLI_16, display_get_width()/2-20, display_get_height()/2+10, retryStr);

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id= currSelection==PAUSE_SELECTION_QUIT ? FONTSTYLE_RED : FONTSTYLE_WHITE,
    }, FONT_PIACEVOLI_16, display_get_width()/2-20, display_get_height()/2+30, quitStr);

    

}