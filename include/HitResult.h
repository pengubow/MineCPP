#pragma once

#include <cstdint>

class HitResult {
public:
    int32_t face;
    int32_t x;
    int32_t y; 
    int32_t z;
    int32_t type;
    
    HitResult(int32_t type, int32_t x, int32_t y, int32_t z, int32_t face);
};