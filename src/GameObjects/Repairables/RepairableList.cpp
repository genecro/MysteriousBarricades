#include "RepairableList.h"

RepairableList::RepairableList() {
    nextId_ = 0;
    repairables = new std::vector<GO_Repairable*>();
}

RepairableList::~RepairableList() {
    for (GO_Repairable* obj : *repairables) {
        delete obj;
    }
    repairables->clear();
    delete repairables;
}

void RepairableList::push(GO_Repairable* newGO) {
    newGO->id_ = nextId_++;
    repairables->push_back(newGO);
    if(!currentIdSet_) {
        currentId_=newGO->id_;
        currentIdSet_ = true;
    }
}

void RepairableList::remove(GO_Repairable* delObj) {
    uint8_t id = delObj->id_;
    for(int i = 0; i < repairables->size(); i++) {
        if(repairables->at(i)->id_ == id) {
            GO_Repairable* tmpObj = repairables->at(i);
            repairables->erase(repairables->begin()+i);
            delete tmpObj;
            break;
        }
    }

    if(currentId_==id) {
        if(repairables->size()) currentId_=repairables->at(0)->id_;
        else currentIdSet_=false;
    }
}

void RepairableList::handleInput() {
    for(GO_Repairable* i: *repairables) {
        i->handleInput();
    }
}

void RepairableList::update() {
    for(GO_Repairable* i: *repairables) {
        i->update();
    }
}

void RepairableList::renderT3d() {
    for(GO_Repairable* i: *repairables) {
        i->renderT3d();
    }
}

void RepairableList::renderRdpq() {
    for(GO_Repairable* i: *repairables) {
        i->renderRdpq();
    }
}

GO_Repairable* RepairableList::getCurrRepairable() {
    if(!currentIdSet_) return NULL;
    for(int i = 0; i < repairables->size(); i++) {
        if(repairables->at(i)->id_ == currentId_) {
            return repairables->at(i);
        }
    }
}

GO_Repairable* RepairableList::getNextRepairable() {
    if(!currentIdSet_) return NULL;
    for(int i = 0; i < repairables->size(); i++) {
        if(repairables->at(i)->id_ == currentId_) {
            i = (i+1) % repairables->size();
            currentId_ = repairables->at(i)->id_;
            return repairables->at(i);
        }
    }

}

GO_Repairable* RepairableList::getPrevRepairable() {
    if(!currentIdSet_) return NULL;
    for(int i = 0; i < repairables->size(); i++) {
        if(repairables->at(i)->id_ == currentId_) {
            i = (i-1) % repairables->size();
            currentId_ = repairables->at(i)->id_;
            return repairables->at(i);
        }
    }
}

GO_Repairable* RepairableList::getRandDamagedRep() {
    std::vector<GO_Repairable*> damagedList{};
    for(auto& r : *repairables) {
        if(!r->fullyRepaired) {
            damagedList.push_back(r);
        }
    }
    if(damagedList.size() == 0) return nullptr;
    return damagedList.at(rand()*damagedList.size()/(RAND_MAX+1));
}

void RepairableList::setCurrentRepairable(GO_Repairable* newCurr) {
    currentId_ = newCurr->id_;
}