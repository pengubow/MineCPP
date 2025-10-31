#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include "sound/Audio.h"
#include "sound/SoundPos.h"
#include "sound/AudioInfo.h"

using namespace std;

#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "Util/miniaudio.h"
#include "sound/Audio.h"
#include "sound/AudioInfo.h"

using namespace std;

class SoundPlayer : public enable_shared_from_this<SoundPlayer> {
public:
    bool running = false;
    bool enabled = false;
    
    SoundPlayer();
    ~SoundPlayer();
    
    void play(shared_ptr<Audio> var1);
    void play(shared_ptr<AudioInfo>& var1, shared_ptr<SoundPos> var2);

    vector<shared_ptr<Audio>> audioQueue;

private:
    ma_device device;
    mutex audioQueueMutex;
    
    static void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    void mix_audio(int16_t* output, ma_uint32 frameCount);
};