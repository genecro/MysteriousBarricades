#include "BarricadeList.h"

BarricadeList::BarricadeList() {
    nextId_ = 0;
    gameObjects_ = new std::vector<GO_Barricade*>();
}

BarricadeList::~BarricadeList() {
    for (GO_Barricade* obj : *gameObjects_) {
        delete obj;
    }
    gameObjects_->clear();
    delete gameObjects_;
}

void BarricadeList::push(GO_Barricade* newGO) {
    newGO->id_ = nextId_++;
    gameObjects_->push_back(newGO);
}

void BarricadeList::remove(GO_Barricade* delObj) {
    uint8_t id = delObj->id_;
    for(int i = 0; i < gameObjects_->size(); i++) {
        if(gameObjects_->at(i)->id_ == id) {
            GO_Barricade* tmpObj = gameObjects_->at(i);
            gameObjects_->erase(gameObjects_->begin()+i);
            delete tmpObj;
            break;
        }
    }
}

void BarricadeList::handleInput() {
    for(GO_Barricade* i: *gameObjects_) {
        i->handleInput();
    }
}

void BarricadeList::update() {
    for(GO_Barricade* i: *gameObjects_) {
        //remove if flagged for removal
        if(i->timeToDelete) remove(i);
        else i->update();
    }
}

void BarricadeList::renderT3d() {
    for(GO_Barricade* i: *gameObjects_) {
        i->renderT3d();
    }
}

void BarricadeList::renderRdpq() {
    for(GO_Barricade* i: *gameObjects_) {
        i->renderRdpq();
    }
}