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
    void update(T3DVec3 cursorPosition);
    void renderT3d();
    void renderRdpq();

    std::vector<GameObject*>* gameObjects;

private:
    
    int nextId_;
};