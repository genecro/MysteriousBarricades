#include "GO_Barricade.h"
#include "../../globals.h"

GO_Barricade::GO_Barricade() {

}

GO_Barricade::~GO_Barricade() {

}

void GO_Barricade::checkTimeLeft() {
    framesUntilDelete -= 1.0f*global::frameTimeMultiplier;

    //start flashing when 3 seconds left
    if(framesUntilDelete < 3*60) {
        objColor_.a = 100.0f*pow(fm_sinf(pow((4.0f*60.0f-framesUntilDelete)/60.0f, 3)), 2) + 27.0f;

        //flag for removal when framesUntilDelete reaches 0
        if(framesUntilDelete <= 0) {
            timeToDelete = true;
        }
    }
}

void GO_Barricade::processProjectile(GO_Projectile* theProjectile) {
    if(theProjectile->justReflected) {
        theProjectile->justReflected = false;
    }
    else {
        if(checkCollision(theProjectile)) {
            theProjectile->angle_ = 2*rotation_ - theProjectile->angle_;
            theProjectile->justReflected = true;
            theProjectile->reflected = true;
            global::audioManager->playSFX("magicStep6.wav64", {.volume = 0.4f});
        }
    }
}

//bool GO_Barricade::checkCollision(GO_Enemy* theEnemy) {
bool GO_Barricade::checkCollision(GameObject* theEnemy) {
    
    float xDiff = position_.x-theEnemy->position_.x;
    float yDiff = position_.z-theEnemy->position_.z;
    
    return abs(
        fm_cosf(rotation_)*(yDiff) 
        - fm_sinf(rotation_)*(xDiff)
    ) <= theEnemy->objectWidth_
    && sqrt(xDiff*xDiff + yDiff*yDiff) <= scale_/scaleFactor_;
}