#pragma once

#include <vector>
#include <memory>
#include "sound/Audio.h"
#include "sound/AudioInfo.h"
#include "sound/SoundPos.h"

using namespace std;

class Sound : public Audio {
private:
    shared_ptr<AudioInfo> info;
    shared_ptr<SoundPos> pos;
    float pitch = 0.0f;
    float volume = 1.0f;
    vector<int16_t> data = vector<int16_t>(4410);

public:
    Sound(shared_ptr<AudioInfo> var1, shared_ptr<SoundPos> var2);
    
    bool play(vector<int32_t>& var1, vector<int32_t>& var2, int32_t var3) override;
};