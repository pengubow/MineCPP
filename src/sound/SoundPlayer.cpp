#include <iostream>
#include "sound/SoundPlayer.h"
#include "sound/Sound.h"

void SoundPlayer::audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (!pDevice || !pDevice->pUserData) return;

    SoundPlayer* pThis = (SoundPlayer*)pDevice->pUserData;
    pThis->mix_audio((int16_t*)pOutput, frameCount);
}

void SoundPlayer::mix_audio(int16_t* output, ma_uint32 frameCount) {
    vector<int32_t> var1(frameCount);
    vector<int32_t> var2(frameCount);
    
    fill(var1.begin(), var1.end(), 0);
    fill(var2.begin(), var2.end(), 0);

    {
        lock_guard<mutex> lock(audioQueueMutex);
        for (int32_t i = (int32_t)audioQueue.size() - 1; i >= 0; --i) {
            if (!audioQueue[i]) continue;
            
            bool var16 = audioQueue[i]->play(var1, var2, frameCount);
            if (!var16) {
                audioQueue.erase(audioQueue.begin() + i);
            }
        }
    }

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        if (!options->music && !options->sound) {
            output[i * 2] = 0;
            output[i * 2 + 1] = 0;
        }
        else {
            int32_t left = var1[i];
            int32_t right = var2[i];
            
            if (left < -32000) left = -32000;
            if (right < -32000) right = -32000;
            if (left >= 32000) left = 32000;
            if (right >= 32000) right = 32000;

            output[i * 2] = (int16_t)left;
            output[i * 2 + 1] = (int16_t)right;
        }
    }
}

SoundPlayer::SoundPlayer(Options* options) 
    : running(false), options(options) {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.sampleRate = 44100;
    config.dataCallback = audio_callback;
    config.pUserData = this;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        cerr << "Failed to initialize audio device" << endl;
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        cerr << "Failed to start audio device" << endl;
        ma_device_uninit(&device);
        return;
    }

    running = true;
}

SoundPlayer::~SoundPlayer() {
    ma_device_stop(&device);
    
    {
        lock_guard<mutex> lock(audioQueueMutex);
        audioQueue.clear();
    }
    
    ma_device_uninit(&device);
}

void SoundPlayer::play(shared_ptr<Audio> var1) {
    if (running) {
        lock_guard<mutex> lock(audioQueueMutex);
        audioQueue.push_back(var1);
    }
}

void SoundPlayer::play(shared_ptr<AudioInfo>& var1, shared_ptr<SoundPos> var2) {
    play(make_shared<Sound>(var1, var2));
}