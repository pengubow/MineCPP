#pragma once

#include "sound/BaseSoundPos.h"

class LevelSoundPos : public BaseSoundPos {
    float x;
    float y;
    float z;
public:
    LevelSoundPos(float x, float y, float z, shared_ptr<Entity> var4);

    float getRotationDiff();
    float getDistanceSq();
};