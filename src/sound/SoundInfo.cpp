#include "sound/SoundInfo.h"

SoundInfo::SoundInfo(shared_ptr<SoundData>& var1, float var2, float var3)
    : data(var1) {
    pitch = var2 * 44100.0f / var1->length;
    volume = var3;
}

int32_t SoundInfo::update(vector<short>& var1, int32_t var2) {
    if (seek >= (float)data->data.size()) {
        return 0;
    }

    for (int var3 = 0; var3 < var2; ++var3) {
        int var4 = (int)seek;
        short var5 = data->data[var4];
        short var6 = var4 < (int)data->data.size() - 1 ? data->data[var4 + 1] : 0;
        var1[var3] = (short)((int)((float)var5 + (float)(var6 - var5) * (seek - (float)var4)));
        seek += pitch;
        if (seek >= (float)data->data.size()) {
            return var3;
        }
    }

    return var2;
}