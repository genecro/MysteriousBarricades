#include "GI_FadeIn.h"

GI_FadeIn::GI_FadeIn() {
    fadeTime_ = 300.0f;
    fadeAlpha_ = 1.0f;
}

GI_FadeIn::GI_FadeIn(float fadeTime) {
    fadeTime_ = fadeTime;
    fadeAlpha_ = 1.0f;
}

GI_FadeIn::~GI_FadeIn() {

}

void GI_FadeIn::handleInput() {
    
}

void GI_FadeIn::update() {
    if(!started_) {
        startTime_ = get_ticks();
        started_ = true;
    }
    timeline_ = (float)(TIMER_MICROS(get_ticks() - startTime_)/1000);
    fadeAlpha_ = 1.0f - timeline_/fadeTime_;

    if(timeline_ >= fadeTime_) {
        timeToDestroy = true;
    }
}

void GI_FadeIn::renderT3d() {

}

void GI_FadeIn::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

    rdpq_set_prim_color(RGBA32(0, 0, 0, fadeAlpha_*255));

    rdpq_fill_rectangle(0, 0, display_get_width(), display_get_height());
}