#pragma once

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <string>
#include <memory>
#include <map>
#include "../collision.h"


class GameObject
{
public:
    GameObject();
    virtual ~GameObject();
    virtual void handleInput() = 0;
    virtual void update() = 0;
    virtual void renderT3d() = 0;
    virtual void renderRdpq() = 0;

    virtual void consumeEffect();
    virtual void activate();

    std::vector<Triangle>* collisionTris;

    void checkCollision();

    T3DVec3 position_;
    float rotation_;
    std::string name_;

    float objectWidth_;
    float stretch_ = 1.0f;

    int id_;

    bool isSolid_;

    float downwardVel;
    float terminalVel;

    bool timeToDelete = false;

    float lifetime_ = 0.0f;

    bool takeable_ = false;
    bool isProjectile_ = false;

    int rotationDirection_ = 1;

    float speed_ = 1.0f;

    int idOfLastReflected_ = -1;
protected:
    bool updateHasBeenCalled_ = false;
};