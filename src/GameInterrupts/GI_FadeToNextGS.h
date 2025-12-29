#pragma once
#include <libdragon.h>
#include "../Fonts.h"
#include "GameInterrupt.h"
#include "../globals.h"
#include "../GameStates/GameState.h"
//#include "../GameStates/GSWrapper.h"

template <typename T> class GI_FadeToNextGS : public GameInterrupt
{
public:
    GI_FadeToNextGS(T3DVec3 playerStartingPos, float fadeTime = 300.0f);
    ~GI_FadeToNextGS();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    uint64_t startTime_;
    float timeline_;

    float fadeAlpha_;
    float fadeTime_;
    bool started_ = false;

    T3DVec3 playerStartingPos_;
};

template <typename T> GI_FadeToNextGS<T>::GI_FadeToNextGS(T3DVec3 playerStartingPos, float fadeTime = 300.0f) {
    fadeTime_ = fadeTime;
    fadeAlpha_ = 1.0f;
    playerStartingPos_ = playerStartingPos;
}

template <typename T> GI_FadeToNextGS<T>::~GI_FadeToNextGS() {

}

template <typename T> void GI_FadeToNextGS<T>::handleInput() {
    
}

template <typename T> void GI_FadeToNextGS<T>::update() {
    if(!started_) {
        started_ = true;
        startTime_ = get_ticks();
    }
    timeline_ = (float)(TIMER_MICROS(get_ticks() - startTime_)/1000.0f);
    fadeAlpha_ = timeline_/fadeTime_;

    if(timeline_ >= fadeTime_) {
        global::gameState->nextState = new T(playerStartingPos_);
        timeToDestroy = true;
    }
}

template <typename T> void GI_FadeToNextGS<T>::renderT3d() {

}

template <typename T> void GI_FadeToNextGS<T>::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

    rdpq_set_prim_color(RGBA32(0, 0, 0, fadeAlpha_*255));

    rdpq_fill_rectangle(0, 0, display_get_width(), display_get_height());
}