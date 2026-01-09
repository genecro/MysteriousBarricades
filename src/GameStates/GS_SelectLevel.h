#pragma once

#include "GameState.h"


class GS_SelectLevel : public GameState {
public:
    GS_SelectLevel(T3DVec3 startingPos);
    ~GS_SelectLevel();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    void checkForWinOrLoss() override;

    void levelWon() override;
    void levelLost() override;

    void barricadeCreated() override;

    enum {
        SELECTION_TRAINING = 0,
        SELECTION_LEVEL_1 = 1,
        SELECTION_LEVEL_2 = 2,
        SELECTION_BOSS1 = 3
    };

    uint8_t currSelection = SELECTION_TRAINING;

    

private:
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};

    uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec;

    
    T3DMat4FP* envMatFP;
    float scaleFactor = 0.05f;

    void initCamera() override;
    void handleInputCamera() override;
    void updateCamera() override;

    uint8_t trainingBoxX = 80;
    uint8_t trainingBoxY = 70;
    uint8_t level01BoxX = 80;
    uint8_t level01BoxY = 110;
    uint8_t level02BoxX = 120;
    uint8_t level02BoxY = 110;
    uint8_t boss1BoxX = 160;
    uint8_t boss1BoxY = 110;
    uint8_t selectBoxSize = 30;
    uint8_t selectBoxBorder = 3;
    
    sprite_t *levelLockedSprite;
};