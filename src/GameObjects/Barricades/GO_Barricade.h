#pragma once

#include "../GameObject.h"
//#include "../globals.h"
#include "../Enemies/GO_Enemy.h"
#include "../GO_Projectile.h"

class GO_Barricade: public GameObject {
public:
    GO_Barricade();

    color_t objColor_ = (color_t){0,0,0,0};

    virtual ~GO_Barricade();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;
    virtual void processEnemy(GO_Enemy* theEnemy) = 0;
    virtual void processProjectile(GO_Projectile* theProjectile);
    virtual void castSuccess() = 0;
    //bool checkCollision(GO_Enemy* theEnemy);
    bool checkCollision(GameObject* theEnemy);

    void checkTimeLeft();

    float framesUntilDelete = 5.0f*60.0f;

    float scale_;
    float scaleFactor_;

    bool castSuccess_ = false;
    bool castHasFailed_ = false;
    float failedTimer_;
    float failedTimerMax_;
};