#include <iostream>
#include <vorbis/vorbisfile.h>
#include "sound/SoundReader.h"

shared_ptr<SoundData> SoundReader::read(string var0) {
    FILE* file = fopen(var0.c_str(), "rb");
    if (!file) {
        cerr << "Failed to open: " << var0 << endl;
        return nullptr;
    }

    OggVorbis_File vf;
    if (ov_open(file, &vf, NULL, 0) < 0) {
        cerr << "Failed to open OGG: " << var0 << endl;
        fclose(file);
        return nullptr;
    }

    vorbis_info* info = ov_info(&vf, -1);
    float sampleRate = (float)info->rate;

    vector<short> var5;
    char pcmout[4096];
    int bitstream;
    long ret;

    while (true) {
        ret = ov_read(&vf, pcmout, sizeof(pcmout), 0, 2, 1, &bitstream);
        if (ret == 0) {
            break;
        }
        if (ret < 0) {
            cerr << "Error reading OGG" << endl;
            break;
        }

        for (long i = 0; i < ret; i += 2) {
            short sample = (short)((pcmout[i + 1] << 8) | (pcmout[i] & 0xFF));
            var5.push_back(sample);
        }
    }

    ov_clear(&vf);

    if (var5.size() > 0) {
        return make_shared<SoundData>(var5, sampleRate);
    }
    return nullptr;
}