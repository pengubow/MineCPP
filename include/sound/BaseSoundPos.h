#pragma once

#include <cmath>
#include <memory>
#include "sound/SoundPos.h"
#include "Entity.h"

using namespace std;

class BaseSoundPos : public SoundPos {
    shared_ptr<Entity> listener;

public:
    BaseSoundPos(shared_ptr<Entity>& var1);
    
    float getRotationDiff(float x, float z);
    float getDistanceSq(float x, float y, float z);
};