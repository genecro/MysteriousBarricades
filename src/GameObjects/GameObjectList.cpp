#include "GameObjectList.h"

GameObjectList::GameObjectList() {
    nextId_ = 0;
    gameObjects = new std::vector<GameObject*>();
}

GameObjectList::~GameObjectList() {
    for (GameObject* obj : *gameObjects) {
        delete obj;
    }
    gameObjects->clear();
    delete gameObjects;
}

void GameObjectList::push(GameObject* newGO) {
    newGO->id_ = nextId_++;
    gameObjects->push_back(newGO);
}

void GameObjectList::remove(GameObject* delObj) {
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

void GameObjectList::handleInput() {
    for(GameObject* i: *gameObjects) {
        i->handleInput();
    }
}

void GameObjectList::update() {
    for(GameObject* i: *gameObjects) {
        i->update();
    }
}

void GameObjectList::renderT3d() {
    for(GameObject* i: *gameObjects) {
        i->renderT3d();
    }
}

void GameObjectList::renderRdpq() {
    for(GameObject* i: *gameObjects) {
        i->renderRdpq();
    }
}