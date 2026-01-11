#pragma once

#include "GameState.h"
#include "../GameObjects/Bosses/GO_Boss1.h"


class GS_Boss1 : public GameState {
public:
    GS_Boss1(T3DVec3 startingCursorPosition);
    ~GS_Boss1();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    void checkForWinOrLoss() override;

    void levelWon() override;
    void levelLost() override;

    GO_Boss1* theBoss_;

    

private:
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};

    uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec;

    
    T3DMat4FP* envMatFP;
    float scaleFactor = 0.05f;

    void initCamera() override;
    void handleInputCamera() override;
    void updateCamera() override;
};