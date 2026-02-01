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
    //if(theProjectile->justReflected) {
    //    theProjectile->justReflected = false;
    //}
    //else {
        if(theProjectile->idOfLastReflected_ != this->id_ && checkCollision(theProjectile)) {
            if(global::gameProgress.barricadesCanRicochet) {
                theProjectile->angle_ = 2*rotation_ - theProjectile->angle_;
                theProjectile->justReflected = true;
                theProjectile->reflected = true;
                global::audioManager->playSFX("magicStep6.wav64", {.volume = 0.4f});
                theProjectile->rotationDirection_ *= -1;
                theProjectile->idOfLastReflected_ = this->id_;
                if(global::gameState->projectilesDestroyBarricades) {
                    timeToDelete = true;
                }
            }
            else {
                global::gameState->projectileNotDeflected();
                theProjectile->timeToDelete = true;
                timeToDelete = true;
            }
        }
    //}
}

//bool GO_Barricade::checkCollision(GO_Enemy* theEnemy) {
bool GO_Barricade::checkCollision(GameObject* theEnemy) {

    float xp = (theEnemy->position_.x - position_.x)*fm_cosf(rotation_) + (theEnemy->position_.z - position_.z)*fm_sinf(rotation_);
    float zp = -(theEnemy->position_.x - position_.x)*fm_sinf(rotation_) + (theEnemy->position_.z - position_.z)*fm_cosf(rotation_);
    float L2 = scale_/scaleFactor_;

    if(theEnemy->stretch_ == 1.0f) {
        float xclamp = clamp(xp, -L2, L2);

        float distance = theEnemy->isProjectile_ ? (theEnemy->objectWidth_ + theEnemy->speed_) * global::frameTimeMultiplier : theEnemy->objectWidth_;

        return (xp - xclamp)*(xp - xclamp) + zp*zp <= distance*distance;
    }

    else {
        //this is a special case for collision with a zone defined by two parallel lines aligned with the x axis
        //need to make it more general in the future
        float z1 = position_.z + scale_/scaleFactor_ * fm_sinf(rotation_);
        float z2 = position_.z - scale_/scaleFactor_ * fm_sinf(rotation_);

        return fmax(z1, z2) >= theEnemy->position_.z - theEnemy->objectWidth_ 
                && fmin(z1, z2) <= theEnemy->position_.z + theEnemy->objectWidth_;
    }
}

float GO_Barricade::clamp(float val, float minVal, float maxVal) {
    if(val < minVal) return minVal;
    if(val > maxVal) return maxVal;
    return val;
}