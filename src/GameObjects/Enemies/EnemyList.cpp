#include "EnemyList.h"

EnemyList::EnemyList(std::vector<Triangle> *collisionTris) {
    nextId_ = 0;
    gameObjects = new std::vector<GO_Enemy*>();
    collisionTris_ = collisionTris;
}

EnemyList::~EnemyList() {
    for (GO_Enemy* obj : *gameObjects) {
        delete obj;
    }
    gameObjects->clear();
    delete gameObjects;
}

void EnemyList::push(GO_Enemy* newGO) {
    newGO->id_ = nextId_++;
    gameObjects->push_back(newGO);
}

void EnemyList::remove(GO_Enemy* delObj) {
    uint8_t id = delObj->id_;
    for(int i = 0; i < gameObjects->size(); i++) {
        if(gameObjects->at(i)->id_ == id) {
            GameObject* tmpObj = gameObjects->at(i);
            gameObjects->erase(gameObjects->begin()+i);
            delete tmpObj;
            break;
        }
    }
}

void EnemyList::handleInput() {
    for(GO_Enemy* i: *gameObjects) {
        i->handleInput();
    }
}

void EnemyList::update() {
    for(GO_Enemy* i: *gameObjects) {
        T3DVec3 floatingPosition = (T3DVec3){i->position_.x, 20, i->position_.z};
        i->setGroundCoord(collision::findGroundIntersection(*collisionTris_, floatingPosition).y);
        i->update();
    }
}

void EnemyList::renderT3d() {
    for(GO_Enemy* i: *gameObjects) {
        i->renderT3d();
    }
}

void EnemyList::renderRdpq() {
    for(GO_Enemy* i: *gameObjects) {
        i->renderRdpq();
    }
}