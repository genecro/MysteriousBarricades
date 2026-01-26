#pragma once

#include "GameState.h"


class GS_Training03 : public GameState {
public:
    GS_Training03(T3DVec3 startingCursorPosition);
    ~GS_Training03();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    void checkForWinOrLoss() override;

    void levelWon() override;
    void levelLost() override;

    void barricadeCastFailed() override;
    void enemiesAttackingStructure() override;
    void enemyDestroyed() override;

    

private:
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};

    uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec;

    bool barricadeHasFailedOnce = false;
    bool alertedEnemiesAttackingOnce = false;
    bool alertedRPBoostOnce = false;
    bool alertedStructureEntryOnce = false;

    T3DMat4FP* envMatFP;
    float scaleFactor = 0.05f;

    void initCamera() override;
    void handleInputCamera() override;
    void updateCamera() override;

    bool introAlertShown = false;
    
};