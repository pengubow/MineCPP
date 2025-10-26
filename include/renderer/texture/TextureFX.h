#pragma once

#include <vector>
#include <stdint.h>

using namespace std;

class TextureFX {
public:
    vector<uint8_t> imageData = vector<uint8_t>(1024);
    int32_t iconIndex;

    TextureFX(int32_t iconIndex);

    virtual void onTick();
};