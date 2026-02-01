#pragma once

#include "GameState.h"
#include <functional>
#include <vector>
#include "../GameObjects/GO_Cannon.h"

struct TimedChallengeEvent {
    float time;
    std::function<bool()> action; //return true if action can be completed
};

struct cannonParms {
    float rot;
    T3DVec3 pos;
};

class GS_Training04 : public GameState {
public:
    GS_Training04(T3DVec3 startingCursorPosition);
    ~GS_Training04();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    void checkForWinOrLoss() override;

    void levelWon() override;
    void levelLost() override;

    void projectileWentOutOfBounds() override;

    

private:
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};

    uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
    T3DVec3 lightDirVec;

    int currentScore = 0;
    
    T3DMat4FP* envMatFP;
    float scaleFactor = 0.05f;

    void initCamera() override;
    void handleInputCamera() override;
    void updateCamera() override;

    color_t colors[3] = {(color_t){0xFF, 0, 0, 0xFF},
                        (color_t){0xFF, 0xFF, 0, 0xFF},
                        (color_t){0, 0, 0xFF, 0xFF}};

    cannonParms cannonPlacement[3] = {{.rot=-T3D_PI, .pos=(T3DVec3){0, 5, 16}}, //center, blue
                                    {.rot=3.0f*T3D_PI/4.0f, .pos=(T3DVec3){11,5,11}}, //left, red
                                    {.rot=-3.0f*T3D_PI/4.0f, .pos=(T3DVec3){-11,5,11}}}; //right, yellow

    std::vector<TimedChallengeEvent> currTimeline;
    std::vector<TimedChallengeEvent> addRandomCannon;

    GO_Cannon* currCannon = nullptr;

    void updateTimeline();
    
};