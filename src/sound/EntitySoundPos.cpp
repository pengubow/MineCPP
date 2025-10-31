#include "sound/EntitySoundPos.h"

EntitySoundPos::EntitySoundPos(shared_ptr<Entity>& var1, shared_ptr<Entity> var2) 
    : BaseSoundPos(var2), source(var1) {}

float EntitySoundPos::getRotationDiff() {
    return BaseSoundPos::getRotationDiff(source->x, source->z);
}

float EntitySoundPos::getDistanceSq() {
    return BaseSoundPos::getDistanceSq(source->x, source->y, source->z);
}