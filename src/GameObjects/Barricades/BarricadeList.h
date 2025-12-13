#pragma once

#include <vector>
#include "GO_Barricade.h"


class BarricadeList
{
public:
    BarricadeList();
    virtual ~BarricadeList();
    void push(GO_Barricade* newGO);
    void remove(GO_Barricade* delObj);
    std::vector<GO_Barricade*>* gameObjects_;
    

    void handleInput();
    void update();
    void renderT3d();
    void renderRdpq();

private:
    
    int nextId_;
};