#include "GO_Boss1.h"
#include "../../globals.h"
#include "../GO_Explosion.h"
#include <math.h>

T3DModel* GO_Boss1::bossModel = nullptr;
uint8_t GO_Boss1::instanceCount = 0;

GO_Boss1::GO_Boss1(T3DVec3 pos, float rotation) {
    position_ = pos;
    rotation_ = rotation;
}