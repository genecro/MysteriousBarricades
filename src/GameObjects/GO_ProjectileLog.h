#pragma once

#include "GameObject.h"

class GO_ProjectileLog : public GameObject {
public:
    GO_ProjectileLog(T3DVec3 position, float angle, float speed, GameObject* origin, float objectWidth, float stretch, float damage=20);
    virtual ~GO_ProjectileLog();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

    bool reflected = false;
    bool justReflected = false;
    float angle_;

    float damage_;

    float stretch_;

    GameObject* origin_;

    int idOfLastReflected_ = -1;


private:
    T3DMat4 projectileMat;
    T3DMat4FP* projectileMatFP;
    static T3DModel* projectileModel;

    float scaleFactor_;

    
    //float speed_;

    static uint8_t instanceCount;

    color_t objColor_;
};