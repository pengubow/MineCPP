#pragma once

#include <vector>
#include "stdint.h"

using namespace std;

class AudioInfo {
public:
    float volume = 1.0f;

    virtual int32_t update(vector<int16_t>& var1, int32_t var2);
};