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
            if(global::gameProgress.barricadesCanRicochet) {
                theProjectile->angle_ = 2*rotation_ - theProjectile->angle_;
                theProjectile->justReflected = true;
                theProjectile->reflected = true;
                global::audioManager->playSFX("magicStep6.wav64", {.volume = 0.4f});
            }
            else {
                theProjectile->timeToDelete = true;
                timeToDelete = true;
            }
        }
    }
}

//bool GO_Barricade::checkCollision(GO_Enemy* theEnemy) {
bool GO_Barricade::checkCollision(GameObject* theEnemy) {

    float xp = (theEnemy->position_.x - position_.x)*fm_cosf(-rotation_) - (theEnemy->position_.z - position_.z)*fm_sinf(-rotation_);
    float zp = (theEnemy->position_.x - position_.x)*fm_sinf(-rotation_) + (theEnemy->position_.z - position_.z)*fm_cosf(-rotation_);

    float xclamp = fmax(-scale_/scaleFactor_, fmin(scale_/scaleFactor_, xp));
    return (xp - xclamp)*(xp - xclamp) + zp*zp <= theEnemy->objectWidth_*theEnemy->objectWidth_;
}