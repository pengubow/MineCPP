#pragma once

#include <cstdint>
#include "player/Player.h"

class HitResult {
public:
    int32_t face;
    int32_t x;
    int32_t y; 
    int32_t z;
    
    HitResult(int32_t var1, int32_t x, int32_t y, int32_t z, int32_t face);

    float distanceTo(Player* player, int32_t editMode);
};