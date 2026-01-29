#pragma once

#include "GameState.h"
//#include "../GameObjects/GO_Player.h"

class GS_Boss1InteriorA : public GameState {
public:
    GS_Boss1InteriorA(T3DVec3 playerStartingPos);
    ~GS_Boss1InteriorA();

    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    //GO_Player* thePlayer_;

private:
    uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};

    T3DViewport viewport;
    T3DMat4FP* envMatFP;

    void initCamera() override;
    void handleInputCamera() override;
    void updateCamera() override;
};