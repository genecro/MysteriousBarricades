#include <stdio.h>

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include "GameStates/GS.h"
#include "globals.h"
#include "Fonts.h"
#include "GameInterrupts/GI_Pause.h"

int main(void)
{
    float smoothedDeltaTime = 1.0f/60.0f;
    timer_init();

    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    
    dfs_init(DFS_DEFAULT_LOCATION);

    //display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);

    //audio_init(48000, 4);
    //mixer_init(16);

    audio_init(32000, 3);
    mixer_init(32);

	// Initialize wav64 compression level 3 as we're going to use it
	//wav64_init_compression(3);

    rdpq_init();
    //rdpq_debug_start();
    Fonts::fonts_init();
    t3d_init((T3DInitParams){});
    //tpx_init((TPXInitParams){});

    global::audioManager = new Audio();

    global::GameInterruptStack = new std::vector<GameInterrupt*>();

    //global::gameState = new GS_Intro();
    //global::gameState = new GS_Menu();
    //global::gameState = new GS_Level01((T3DVec3){0, 10, 0});
    //global::gameState = new GS_Training02((T3DVec3){0, 10, 0});
    global::gameState = new GS_Training03InteriorA((T3DVec3){0,0,0});

    //zbuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;    

    //display_set_fps_limit(30);

    heap_stats_t heapStats = {};

    joypad_init();

    

    debugf("Entering main loop\n");
    while(1) {
        smoothedDeltaTime = fm_lerp(smoothedDeltaTime, fminf(display_get_delta_time(), 0.04f), 0.1f);
        global::frameTimeMultiplier = smoothedDeltaTime*60.0f;
        //debugf("Delta time: %.7f\n", global::frameTimeMultiplier);

        global::disp = display_get();
        surface_t* zbuf = display_get_zbuf();
        //rdpq_attach(disp, &zbuffer);
        rdpq_attach_clear(global::disp, zbuf);
        
        joypad_poll();

        mixer_try_play();

        //remove interrupts that are finished
        for (int i = 0; i < global::GameInterruptStack->size(); i++) {
            if(global::GameInterruptStack->at(i)->isTimeToDestroy()) {
                GameInterrupt* tmpIRP = global::GameInterruptStack->at(i);
                global::GameInterruptStack->erase(global::GameInterruptStack->begin()+i);
                delete tmpIRP;
                i--;
            }
        }

        //handle input from the topmost interrupt or the main state if no interrupts
        if(global::GameInterruptStack->size()>0) {
            global::GameInterruptStack->back()->handleInput();
        }
        else {
            global::gameState->handleInput();
        }

        //look for topmost pause interrupt
        int pauseIndex = -1;
        for (int i = global::GameInterruptStack->size()-1; i >= 0; i--) {
            if(global::GameInterruptStack->at(i)->isPauseInterrupt()) {
                pauseIndex = i;
                break;
            }
        }

        //if no pauses found, update all layers
        if (pauseIndex == -1) {
            global::gameState->update();
            for(int i = 0; i < global::GameInterruptStack->size(); i++) {
                global::GameInterruptStack->at(i)->update();
            }
        }
        //otherwise only update topmost pause layer and any above it
        else {
            for(int i = pauseIndex; i < global::GameInterruptStack->size(); i++) {
                global::GameInterruptStack->at(i)->update();
            }
        }

        //render t3d of all layers starting with the main gameState
        t3d_frame_start();
        global::gameState->renderT3d();
        for(int i = 0; i < global::GameInterruptStack->size(); i++) {
            global::GameInterruptStack->at(i)->renderT3d();
        }

        mixer_try_play();

        //render rdpq of all layers starting with the main gameState
        global::gameState->renderRdpq();
        for(int i = 0; i < global::GameInterruptStack->size(); i++) {
            global::GameInterruptStack->at(i)->renderRdpq();
        }

        rdpq_sync_pipe();
        //display stats
        /*
        
        if(global::thePlayer) {
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, 25, 25, "Player position:");
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, 25, 37, "X: %.2f", global::thePlayer->position_.x);
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, 25, 49, "Y: %.2f", global::thePlayer->position_.y);
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, 25, 61, "Z: %.2f", global::thePlayer->position_.z);
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, 25, 73, "ROT: %.2f", global::thePlayer->rotation_);
        }
            */
        //rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, display_get_height()-52, "%.2f FPS", display_get_fps());
        sys_get_heap_stats(&heapStats);
        rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, 25, "%.2f FPS", display_get_fps());
        rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, 37, "Total: %d", heapStats.total);
        rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, 49, "Used:  %d", heapStats.used);
        rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, 61, "Ratio: %.2f%%", 100.0f*heapStats.used/heapStats.total);
        

        

        //if current gameState has nextState, point to nextState and delete current gameState
        if(global::gameState->nextState != nullptr)
        {
            debugf("Next state detected\n");
            GameState* nextState = global::gameState->nextState;
            rspq_wait();
            delete global::gameState;
            global::gameState = nextState;
            debugf("Next state has been set\n");
        }

        if(global::gameState && global::gameState->envModel) {
            rdpq_text_printf(&(rdpq_textparms_t){}, FONT_FREE_12, display_get_width()-110, 73, "EnvVertCt: %d", global::gameState->envModel->totalVertCount);
        }

        rdpq_detach_show();
    }
}