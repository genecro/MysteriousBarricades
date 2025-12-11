#pragma once

#include <vector>
#include "GameObject.h"

class GameObjectList
{
public:
    GameObjectList();
    virtual ~GameObjectList();
    void push(GameObject* newGO);
    void remove(GameObject* delObj);

    void handleInput();
    void update();
    void renderT3d();
    void renderRdpq();

private:
    std::vector<GameObject*>* gameObjects;
    int nextId_;
};