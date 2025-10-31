#pragma once

#include <memory>
#include <vector>
#include "sound/AudioInfo.h"
#include "sound/SoundData.h"

using namespace std;

class SoundInfo : public AudioInfo {
private:
    shared_ptr<SoundData> data;
    float seek = 0.0f;
    float pitch;

public:
    SoundInfo(shared_ptr<SoundData>& var1, float var2, float var3);
    
    int32_t update(vector<int16_t>& var1, int32_t var2) override;
};