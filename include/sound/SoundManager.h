#pragma once

#include <map>
#include "sound/AudioInfo.h"
#include "sound/SoundPlayer.h"
#include "sound/SoundData.h"
#include "Timer.h"
#include "Util/Random.h"

class SoundManager {
public:
    map<string, vector<shared_ptr<SoundData>>> sounds;
    Random random = Random();
    int64_t lastMusic = Timer::nanoTime() / 1000000 + 60000;
private:
    map<string, vector<string>> music;
public:
    SoundManager();
    
    shared_ptr<AudioInfo> getAudioInfo(const string& var1, float var2, float var3);
    void registerSound(const string& var1, const string& filename);
    void registerMusic(const string& filename, const string& var1);
    bool playMusic(shared_ptr<SoundPlayer>& var1, const string& var2);
private:
    string cleanSoundName(string var1);
public:
    void registerSoundsFromDirectory(string dir);
    void registerMusicFromDirectory(string dir);
};