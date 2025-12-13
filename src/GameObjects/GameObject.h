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

    std::vector<Triangle>* collisionTris;

    //void checkCollision();

    T3DVec3 position_;
    float rotation_;
    std::string name_;

    float objectWidth_;

    int id_;

    bool isSolid_;

    float downwardVel;
    float terminalVel;

    bool timeToDelete = false;
};