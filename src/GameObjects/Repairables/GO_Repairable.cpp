#include "GO_Repairable.h"
#include "../../globals.h"

T3DModel* GO_Repairable::repelRingModel = nullptr;
uint8_t GO_Repairable::rRingInstanceCount = 0;

GO_Repairable::GO_Repairable() {
    t3d_mat4_identity(repelRingMat);
    repelRingMatFP = (T3DMat4FP*)malloc_uncached(sizeof(T3DMat4FP));
    repelRingScale = 1.0f;

    rRingInstanceCount++;
    if(!repelRingModel) {
        repelRingModel = t3d_model_load("rom:/repelRing.t3dm");
    }

    /*
    rspq_block_begin();
    t3d_model_draw(repelRingModel);
    dplRepelRing = rspq_block_end();
    */
}

GO_Repairable::~GO_Repairable() {
    free_uncached(repelRingMatFP);
    rRingInstanceCount--;
    if(rRingInstanceCount == 0) {
        t3d_model_free(repelRingModel);
        repelRingModel = nullptr;
    }

    //rspq_block_free(dplRepelRing);
}

void GO_Repairable::updateHPBar() {

    HPBarTotalLength_ = HPTotal_*40.0f/100.0f;

    if(HPCurrent_ >= HPTotal_) {
        if(!fullyRepaired) global::audioManager->playSFX("complexRise2.wav64", {.volume = 0.4f});
        fullyRepaired = true;
        HPCurrent_ = HPTotal_;
    }

    float hpRatio = (float)HPCurrent_/(float)HPTotal_;

    HPBarCurrentLength_ = HPCurrent_ > 0 ? fmax(HPBarTotalLength_*hpRatio-1.0f, 2.0f) : 0;
    
    if(hpRatio <= 0.25) HPBarColor_ = (color_t){0xFF, 0, 0, 0xFF};
    else if(hpRatio <= 0.5) HPBarColor_ = (color_t){0xFF, 0x7F, 0, 0xFF};
    else if(hpRatio <= 0.75) HPBarColor_ = (color_t){0xFF, 0xFF, 0, 0xFF};
    else if(hpRatio < 1) HPBarColor_ = (color_t){0, 0xFF, 0, 0xFF};
    else HPBarColor_ = (color_t){0, 0, 0xFF, 0xFF};

    t3d_viewport_calc_viewspace_pos(global::gameState->viewport, HPBarPos_, position_);
}

void GO_Repairable::drawHPBar() {
    rdpq_sync_pipe();
    rdpq_set_mode_fill(RGBA32(0x00, 0x00, 0x00, 0xFF));
    rdpq_fill_rectangle(HPBarPos_.v[0], HPBarPos_.v[1], HPBarPos_.v[0]+HPBarTotalLength_, HPBarPos_.v[1]+HPBarHeight_);

    rdpq_set_mode_fill(HPBarColor_);
    rdpq_fill_rectangle(HPBarPos_.v[0]+1, HPBarPos_.v[1]+1, HPBarPos_.v[0]+HPBarCurrentLength_, HPBarPos_.v[1]+HPBarHeight_-1);
}

void GO_Repairable::updateRepelRing() {
    repelRingRotation += 0.007;
    t3d_mat4_from_srt_euler(&repelRingMat,
        (float[3]){0.08f*repelRingScale, 0.08f, 0.08f*repelRingScale},
        (float[3]){0.0f, repelRingRotation, 0.0f},
        position_.v
    );
    t3d_mat4_to_fixed(repelRingMatFP, &repelRingMat);
}

void GO_Repairable::renderRepelRing() {
    rdpq_set_prim_color((color_t){0x00, 0xFF, 0x00, 0xFF});
    t3d_matrix_set(repelRingMatFP, true);
    t3d_model_draw(repelRingModel);
    //rspq_block_run(dplRepelRing);
}

void GO_Repairable::processProjectile(GO_Projectile* theProjectile) {

}

GO_Repairable* GO_Repairable::setRewardFunction(std::function<void()> rewardFunction, bool rewardAlreadyReceived) {
    rewardFunction_ = rewardFunction;
    containsReward_ = true;
    rewardAlreadyReceived_ = rewardAlreadyReceived;
    return this;
}

void GO_Repairable::drawRewardIndicator() {
    if(containsReward_) {
        T3DVec3 output;
        t3d_viewport_calc_viewspace_pos(global::gameState->viewport, output, position_+(T3DVec3){0,height_,0});

        rdpq_sync_pipe();
        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(1);

        rdpq_sprite_blit(rewardAlreadyReceived_ ? rewardAlreadyReceivedSprite : fullyRepaired ? rewardIndicatorSprite : rewardIndicatorNotFullyRepairedSprite,
            output.x, output.y,
            &(rdpq_blitparms_t){});
    }
}