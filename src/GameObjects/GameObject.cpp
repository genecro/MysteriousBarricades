//#include "GameObject.h"
//#include "../collision.h"
#include "../globals.h"
#include "../collision.h"

GameObject::GameObject() {
    isSolid_ = false;

}

GameObject::~GameObject() {
    
}

void GameObject::checkCollision() {
    if(isSolid_) {
        AABB objectAABB[1] = {{.min={position_.x-objectWidth_, position_.z-objectWidth_}, .max={position_.x+objectWidth_, position_.z+objectWidth_}}};
        resolve_aabb_collision(objectAABB, &global::gameState->thePlayer_->position_);
    }
}
    

void GameObject::consumeEffect() {
    
}

void GameObject::activate() {

}