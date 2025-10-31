#include <filesystem>
#include <algorithm>
#include "sound/SoundManager.h"
#include "sound/SoundReader.h"
#include "sound/SoundInfo.h"
#include "sound/Music.h"
#include <iostream>
namespace fs = filesystem;

SoundManager::SoundManager() {}

shared_ptr<AudioInfo> SoundManager::getAudioInfo(const string& var1, float var2, float var3) {
    if (sounds.find(var1) == sounds.end()) {
        return nullptr;
    }

    auto& var4 = sounds[var1];
    if (var4.empty()) {
        return nullptr;
    }

    int32_t randomIndex = random.nextInt(var4.size());
    auto var7 = var4[randomIndex];
    return make_shared<SoundInfo>(var7, var3, var2);
}

void SoundManager::registerSound(const string& var1, const string& filename) {
    try {
        string cleanName = cleanSoundName(filename);

        auto var7 = SoundReader::read(var1);
        if (!var7) return;

        if (sounds.find(cleanName) == sounds.end()) {
            sounds[cleanName] = vector<shared_ptr<SoundData>>();
        }

        sounds[cleanName].push_back(var7);
    } catch (const exception& var6) {
    }
}

void SoundManager::registerMusic(const string& filename, const string& var1) {
    try {
        string cleanName = cleanSoundName(filename);
        if (music.find(cleanName) == music.end()) {
            music[cleanName] = vector<string>();
        }

        music[cleanName].push_back(var1);
    } catch (const exception& var6) {
    }
}

bool SoundManager::playMusic(shared_ptr<SoundPlayer>& var1, const string& var2) {
    if (music.find(var2) == music.end()) {
        return false;
    }

    auto& var3 = music[var2];
    if (var3.empty()) {
        return false;
    }

    int32_t randomIndex = random.nextInt(var3.size());
    const string& var8 = var3[randomIndex];
    
    try {
        auto musicPtr = make_shared<Music>(var1, var8);
        var1->play(musicPtr);
        return true;
    } catch (const exception& var6) {
        return false;
    }
}

string SoundManager::cleanSoundName(string var1) {
    var1 = var1.substr(0, var1.length() - 4);

    size_t pos = 0;
    while ((pos = var1.find("/", pos)) != string::npos) {
        var1.replace(pos, 1, ".");
        pos += 1;
    }

    while (!var1.empty() && isdigit(var1.back())) {
        var1.pop_back();
    }

    return var1;
}

void SoundManager::registerSoundsFromDirectory(string dir) {
    string dirName = fs::path(dir).filename().string();
    
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            string fileName = entry.path().filename().string();
            string soundKey = dirName + "." + fileName;
            registerSound(entry.path().string(), soundKey);
        }
    }
}

void SoundManager::registerMusicFromDirectory(string dir) {    
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            string fileName = entry.path().filename().string();
            registerMusic(fileName, entry.path().string());
        }
    }
}