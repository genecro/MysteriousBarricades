#pragma once
#include "GameObject.h"
#include "../GameInterrupts/GI.h"
#include "../globals.h"

#define PADDING 5
#define TOUCHING_DISTANCE 1

template <typename T> class GO_Portal : public GameObject
{
public:
    GO_Portal(T3DVec3 position, float rotation, float width, T3DVec3 playerStartingPos);
    ~GO_Portal();
    virtual void handleInput() override;
    virtual void update() override;
    virtual void renderT3d() override;
    virtual void renderRdpq() override;

protected:
    bool playerContact();
    float xa, ya, xb, yb, xmax, xmin, ymax, ymin, A, B, C;
    T3DVec3 playerStartingPos_;
};

template <typename T> GO_Portal<T>::GO_Portal(T3DVec3 position, float rotation, float width, T3DVec3 playerStartingPos) {
    position_ = position;
    rotation_ = rotation;
    objectWidth_ = width;
    playerStartingPos_ = playerStartingPos;

    float halfWidth = objectWidth_/2.0f;

    xa = position_.x + halfWidth*fm_cosf(rotation);
    ya = position_.z + halfWidth*fm_sinf(rotation);
    xb = position_.x - halfWidth*fm_cosf(rotation);
    yb = position_.z - halfWidth*fm_sinf(rotation);

    A = ya - yb;
    B = xb - xa;
    C = xa*yb - xb*ya;

    if(xb > xa) {
        xmax = xb;
        xmin = xa;
    }
    else {
        xmax = xa;
        xmin = xb;
    }

    if (yb > ya) {
        ymax = yb;
        ymin = ya;
    }
    else {
        ymax = ya;
        ymin = yb;
    }
}

template <typename T> GO_Portal<T>::~GO_Portal() {

}

template <typename T> void GO_Portal<T>::handleInput() {
    if(playerContact()) {
        global::GameInterruptStack->push_back(new GI_FadeToNextGS<T>(playerStartingPos_));
    }
}

template <typename T> void GO_Portal<T>::update() {
    
}

template <typename T> void GO_Portal<T>::renderT3d() {

}

template <typename T> void GO_Portal<T>::renderRdpq() {

}

template <typename T> bool GO_Portal<T>::playerContact() {
    float xp = global::gameState->thePlayer_->position_.x;
    float yp = global::gameState->thePlayer_->position_.z;

    if(xp <= xmax+PADDING && xp >= xmin-PADDING && yp <= ymax+PADDING && yp >= ymin-PADDING) {
        return abs(A*xp+B*yp+C)/objectWidth_ <= TOUCHING_DISTANCE;
    }
    return false;
}