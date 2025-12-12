#pragma once

#include <vector>
#include "GO_Enemy.h"

class EnemyList
{
public:
    EnemyList(std::vector<Triangle> *collisionTris);
    virtual ~EnemyList();
    void push(GO_Enemy* newGO);
    void remove(GO_Enemy* delObj);

    void handleInput();
    void update();
    void renderT3d();
    void renderRdpq();

private:
    std::vector<GO_Enemy*>* gameObjects;
    int nextId_;
    std::vector<Triangle>* collisionTris_;
};