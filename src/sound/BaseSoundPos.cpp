#include "sound/BaseSoundPos.h"

BaseSoundPos::BaseSoundPos(shared_ptr<Entity>& var1) 
    : listener(var1) {}

float BaseSoundPos::getRotationDiff(float x, float z) {    
    x -= listener->x;
    z -= listener->z;
    float var3 = (float)sqrt((double)(x * x + z * z));
    x /= var3;
    z /= var3;
    var3 /= 2.0f;
    if (var3 > 1.0f) {
        var3 = 1.0f;
    }

    float var4 = (float)cos((double)(-listener->yRot) * M_PI / 180.0 + M_PI);
    float var5 = (float)sin((double)(-listener->yRot) * M_PI / 180.0 + M_PI);
    return (var5 * z - var4 * x) * var3;
}

float BaseSoundPos::getDistanceSq(float x, float y, float z) {
    x -= listener->x;
    y -= listener->y;
    float var4 = z - listener->z;
    var4 = (float)sqrt((double)(x * x + y * y + var4 * var4));
    var4 = 1.0f - var4 / 32.0f;
    if (var4 < 0.0f) {
        var4 = 0.0f;
    }

    return var4;
}