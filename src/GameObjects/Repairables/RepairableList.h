#pragma once

#include <vector>
#include "GO_Repairable.h"

class RepairableList
{
public:
    RepairableList();
    virtual ~RepairableList();
    void push(GO_Repairable* newGO);
    void remove(GO_Repairable* delObj);
    GO_Repairable* getCurrRepairable();
    GO_Repairable* getNextRepairable();
    GO_Repairable* getPrevRepairable();
    void setCurrentRepairable(GO_Repairable* newCurr);

    void handleInput();
    void update();
    void renderT3d();
    void renderRdpq();

    std::vector<GO_Repairable*>* repairables;

private:
    
    int nextId_;
    int currentId_;
    bool currentIdSet_ = false;
};