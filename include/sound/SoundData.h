#pragma once

#include <vector>
#include "stdint.h"

using namespace std;

class SoundData {
public:
    vector<int16_t> data;
    float length;

    SoundData(vector<int16_t>& data, float length);
};