//#include "GS_Intro.h"
#include "GS.h"

GS_Intro::GS_Intro() {
    //start_time = timer_ticks();
    //start_time = get_ticks();
    i = 0;
    timeline = 0.0f;
    currSprite = sprite_load(sprites[i]);
    assertf(currSprite, "missing first sprite");
    sprParams.cx = currSprite->width/2.0;
    sprParams.cy = currSprite->height/2.0;

    fadingIn = true;
    fadingOut = false;
    spriteAlpha = 0.0f;
}

GS_Intro::~GS_Intro() {
    sprite_free(currSprite);   
}

void GS_Intro::handleInput() {

}

void GS_Intro::update() {
    if(firstUpdate) {
        firstUpdate = false;
        //start_time = timer_ticks();
        start_time = get_ticks();
        //debugf("Got ticks 30\n");
    }
    //timeline = (float)(TIMER_MICROS(timer_ticks() - start_time)/1000);
    timeline = (float)(TIMER_MICROS(get_ticks() - start_time)/1000);

    //debugf("timeline: %.2f\n", timeline);

    if(fadingIn) {
        if(timeline >= INTRO_FADE_TIME) {
            spriteAlpha = 1.0f;
            fadingIn = false;
            timeline = 0.0f;
            //start_time = timer_ticks();
            start_time = get_ticks();
            //debugf("Got ticks 44\n");
            //debugf("Reached end of fade in\n");
        }
        else {
            spriteAlpha = timeline/INTRO_FADE_TIME;
        }
    }

    else if(fadingOut) {
        //done fading out - determine if more logos need to be displayed
        if(timeline >= INTRO_FADE_TIME) {
            i++;
            //debugf("Reached end of fade out\n");

            //all logos have been displayed
            if(i >= NUM_INTRO_SPRITES) {
                //debugf("Going to main menu\n");
                nextState = new GS_MainMenu();
            }
            //more logo(s) need to be displayed
            else {
                //debugf("Loading next logo\n");
                //start_time = timer_ticks();
                start_time = get_ticks();
                //debugf("Got ticks 67\n");
                timeline = 0.0f;
                sprite_free(currSprite);
                //debugf("freed sprite 71\n");
                currSprite = sprite_load(sprites[i]);
                //debugf("loaded sprite 73\n");
                assertf(currSprite, "missing sprite");
                //debugf("asserted sprite exists\n");
                sprParams.cx = currSprite->width/2;
                sprParams.cy = currSprite->height/2;
                spriteAlpha = 0.0f;
                fadingOut = false;
                fadingIn = true;
            }
        }
        else {
            spriteAlpha = 1.0f - timeline/INTRO_FADE_TIME;
        }
    }

    else {
        if(timeline >= INTRO_STEP_TIME) {
            //debugf("Reached end of display time. Fading out\n");
            fadingOut = true;
            //start_time = timer_ticks();
            start_time = get_ticks();
            //debugf("Got ticks 91\n");
            timeline = 0.0f;
        }
    }
}

void GS_Intro::renderT3d() {

}

void GS_Intro::renderRdpq() {
    rdpq_sync_pipe();
    //debugf("setting mode\n");
    rdpq_set_mode_standard();
    //debugf("setting alpha compare\n");
    rdpq_mode_alphacompare(1);
    //debugf("setting mode blender\n");
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);
    //debugf("setting fog color\n");
    rdpq_set_fog_color(RGBA32(0,0,0,spriteAlpha*255));
    //debugf("blitting sprite\n");
    rdpq_sprite_blit(currSprite, display_get_width()/2, display_get_height()/2, &sprParams);
    //debugf("finished blitting sprite\n");
}

void GS_Intro::testFunc() { 
    debugf("Test func intro\n");
}