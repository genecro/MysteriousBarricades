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