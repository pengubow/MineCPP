#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <vorbis/vorbisfile.h>
#include "sound/Audio.h"

using namespace std;

class SoundPlayer;

class Music : public Audio {
public:
    shared_ptr<SoundPlayer> player;
    bool finished = false;
    
    Music(shared_ptr<SoundPlayer>& var1, const string& filepath);
    
    bool play(vector<int32_t>& var1, vector<int32_t>& var2, int32_t var3) override;

private:
    OggVorbis_File* vf = nullptr;
    FILE* file = nullptr;
    queue<vector<short>> decodedChunks;
    mutex chunkMutex;
    bool decodeFinished = false;
    vector<short> currentChunk;
    int currentChunkPos = 0;
};