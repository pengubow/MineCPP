#pragma once

#include <cstdint>

using namespace std;

class LevelListener {
public:
    virtual void tileChanged(int32_t x, int32_t y, int32_t z) = 0;

    virtual void lightColumnChanged(int32_t x, int32_t z, int32_t minY, int32_t maxY) = 0;

    virtual void allChanged() = 0;
};