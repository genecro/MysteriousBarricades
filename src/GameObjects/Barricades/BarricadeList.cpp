#include "BarricadeList.h"

BarricadeList::BarricadeList() {
    nextId_ = 0;
    gameObjects = new std::vector<GO_Barricade*>();
}

BarricadeList::~BarricadeList() {
    for (GO_Barricade* obj : *gameObjects) {
        delete obj;
    }
    gameObjects->clear();
    delete gameObjects;
}

void BarricadeList::push(GO_Barricade* newGO) {
    newGO->id_ = nextId_++;
    gameObjects->push_back(newGO);
}

void BarricadeList::remove(GO_Barricade* delObj) {
    uint8_t id = delObj->id_;
    for(int i = 0; i < gameObjects->size(); i++) {
        if(gameObjects->at(i)->id_ == id) {
            GO_Barricade* tmpObj = gameObjects->at(i);
            gameObjects->erase(gameObjects->begin()+i);
            delete tmpObj;
            break;
        }
    }
}

void BarricadeList::handleInput() {
    for(GO_Barricade* i: *gameObjects) {
        i->handleInput();
    }
}

void BarricadeList::update() {
    for(GO_Barricade* i: *gameObjects) {
        //remove if flagged for removal
        if(i->timeToDelete) remove(i);
        else i->update();
    }
}

void BarricadeList::renderT3d() {
    for(GO_Barricade* i: *gameObjects) {
        i->renderT3d();
    }
}

void BarricadeList::renderRdpq() {
    for(GO_Barricade* i: *gameObjects) {
        i->renderRdpq();
    }
}