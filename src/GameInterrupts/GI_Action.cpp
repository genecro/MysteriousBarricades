#include <algorithm>
#include "GI_Action.h"
#include "../globals.h"

#define SCREEN_PADDING 25
#define BOX_HEIGHT 75

GI_Action::GI_Action(std::function<void ()> theFunction) {
    theFunction_ = theFunction;

    pauseInterrupt = false;
}

GI_Action::~GI_Action() {

}

void GI_Action::handleInput() {
    
}

void GI_Action::update() {
    theFunction_();
    timeToDestroy = true;
}

void GI_Action::renderT3d() {

}

void GI_Action::renderRdpq() {
    
}