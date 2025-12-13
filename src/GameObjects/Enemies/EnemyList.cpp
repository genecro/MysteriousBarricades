#include "EnemyList.h"

EnemyList::EnemyList(std::vector<Triangle> *collisionTris) {
    nextId_ = 0;
    gameObjects_ = new std::vector<GO_Enemy*>();
    collisionTris_ = collisionTris;
}

EnemyList::~EnemyList() {
    for (GO_Enemy* obj : *gameObjects_) {
        delete obj;
    }
    gameObjects_->clear();
    delete gameObjects_;
}

void EnemyList::push(GO_Enemy* newGO) {
    newGO->id_ = nextId_++;
    gameObjects_->push_back(newGO);
}

void EnemyList::remove(GO_Enemy* delObj) {
    uint8_t id = delObj->id_;
    for(int i = 0; i < gameObjects_->size(); i++) {
        if(gameObjects_->at(i)->id_ == id) {
            GameObject* tmpObj = gameObjects_->at(i);
            gameObjects_->erase(gameObjects_->begin()+i);
            delete tmpObj;
            break;
        }
    }
}

void EnemyList::handleInput() {
    for(GO_Enemy* i: *gameObjects_) {
        i->handleInput();
    }
}

void EnemyList::update() {
    for(GO_Enemy* i: *gameObjects_) {
        //remove if flagged for removal
        if(i->timeToDelete) remove(i);
        else{
            T3DVec3 floatingPosition = (T3DVec3){i->position_.x, 20, i->position_.z};
            i->setGroundCoord(collision::findGroundIntersection(*collisionTris_, floatingPosition).y);
            i->update();
        }
    }
}

void EnemyList::renderT3d() {
    for(GO_Enemy* i: *gameObjects_) {
        i->renderT3d();
    }
}

void EnemyList::renderRdpq() {
    for(GO_Enemy* i: *gameObjects_) {
        i->renderRdpq();
    }
}