#include "sound/LevelSoundPos.h"

LevelSoundPos::LevelSoundPos(float x, float y, float z, shared_ptr<Entity> var4)
    : BaseSoundPos(var4), x(x), y(y), z(z) {};

float LevelSoundPos::getRotationDiff() {
    return BaseSoundPos::getRotationDiff(x, z);
}

float LevelSoundPos::getDistanceSq() {
    return BaseSoundPos::getDistanceSq(x, y, z);
}