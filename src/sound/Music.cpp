#include <iostream>
#include <thread>
#include <vorbis/vorbisfile.h>
#include "sound/Music.h"

Music::Music(shared_ptr<SoundPlayer>& var1, const string& filepath)
    : player(var1), finished(false) {
    
    file = fopen(filepath.c_str(), "rb");
    if (!file) {
        cerr << "Failed to open: " << filepath << endl;
        finished = true;
        return;
    }
    
    vf = (OggVorbis_File*)calloc(1, sizeof(OggVorbis_File));
    if (ov_open(file, vf, NULL, 0) < 0) {
        cerr << "Failed to open OGG: " << filepath << endl;
        fclose(file);
        free(vf);
        vf = nullptr;
        finished = true;
        return;
    }

    thread([this]() {
        char buffer[4096];
        int bitstream;
        long bytesRead;
        vector<short> bigChunk;
        int chunkCount = 0;
        
        while ((bytesRead = ov_read(vf, buffer, sizeof(buffer), 0, 2, 1, &bitstream)) > 0) {
            for (long i = 0; i < bytesRead; i += 2) {
                short sample = (short)((buffer[i + 1] << 8) | (buffer[i] & 0xFF));
                bigChunk.push_back(sample);
            }
            
            if (bigChunk.size() >= 44100 * 2) {
                lock_guard<mutex> lock(chunkMutex);
                decodedChunks.push(bigChunk);
                bigChunk.clear();
            }
        }
        
        if (!bigChunk.empty()) {
            lock_guard<mutex> lock(chunkMutex);
            decodedChunks.push(bigChunk);
        }
        
        {
            lock_guard<mutex> lock(chunkMutex);
            decodeFinished = true;
        }
    }).detach();
}

bool Music::play(vector<int32_t>& var1, vector<int32_t>& var2, int32_t var3) {
    {
        lock_guard<mutex> lock(chunkMutex);
        if (currentChunk.empty() && !decodedChunks.empty()) {
            currentChunk = decodedChunks.front();
            decodedChunks.pop();
            currentChunkPos = 0;
        }
    }

    for (int i = 0; i < var3; ++i) {
        if (currentChunkPos % 2 != 0) {
            currentChunkPos++;
        }
        
        if (currentChunkPos >= (int)currentChunk.size()) {
            {
                lock_guard<mutex> lock(chunkMutex);
                if (!decodedChunks.empty()) {
                    currentChunk = decodedChunks.front();
                    decodedChunks.pop();
                    currentChunkPos = 0;
                } else {
                    currentChunk.clear();
                    break;
                }
            }
        }

        if (currentChunkPos + 1 < (int)currentChunk.size()) {
            var1[i] += currentChunk[currentChunkPos++];
            var2[i] += currentChunk[currentChunkPos++];
        }
    }
    
    bool hasChunks;
    {
        lock_guard<mutex> lock(chunkMutex);
        hasChunks = !decodedChunks.empty() || !currentChunk.empty();
    }
    
    return hasChunks || !decodeFinished;
}