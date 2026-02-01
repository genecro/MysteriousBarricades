#pragma once

#include "GameObject.h"

class GO_Projectile : public GameObject {
public:
    GO_Projectile(T3DVec3 position, float angle, float speed, GameObject* origin, float objectWidth, float damage, color_t objColor = color_t{0xFF, 0, 0, 0xFF});
    virtual ~GO_Projectile();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    bool reflected = false;
    bool justReflected = false;
    float angle_;

    float damage_;

    GameObject* origin_;

    //int idOfLastReflected_ = -1;
    color_t objColor_;


private:
    T3DMat4 projectileMat;
    T3DMat4FP* projectileMatFP;
    static T3DModel* projectileModel;

    float scaleFactor_;

    
    //float speed_;

    static uint8_t instanceCount;

    
};