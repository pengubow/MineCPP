#include "sound/Sound.h"

Sound::Sound(shared_ptr<AudioInfo> var1, shared_ptr<SoundPos> var2)
    : info(var1), pos(var2) {
    pitch = var2->getRotationDiff();
    volume = var2->getDistanceSq() * var1->volume;
}

bool Sound::play(vector<int32_t>& var1, vector<int32_t>& var2, int32_t var3) {
    if ((int32_t)data.size() < var3) {
        data.resize(var3);
    }

    int var4 = info->update(data, var3);
    bool var5 = var4 > 0;
    if (!var5) {
        return false;
    }
    float var6 = pos->getRotationDiff();
    float var7 = pos->getDistanceSq() * info->volume;
    int var8 = (int)((pitch > 0.0f ? 1.0f - pitch : 1.0f) * volume * 65536.0f);
    int var9 = (int)((pitch < 0.0f ? 1.0f + pitch : 1.0f) * volume * 65536.0f);
    int var10 = (int)((var6 > 0.0f ? 1.0f - var6 : 1.0f) * var7 * 65536.0f);
    int var11 = (int)((var6 < 0.0f ? var6 + 1.0f : 1.0f) * var7 * 65536.0f);
    var10 -= var8;
    var11 -= var9;
    int var12, var13, var14;
    if (var10 == 0 && var11 == 0) {
        if (var8 >= 0 || var9 != 0) {
            var12 = var8;
            var13 = var9;

            for (var14 = 0; var14 < var4; ++var14) {
                var1[var14] += data[var14] * var12 >> 16;
                var2[var14] += data[var14] * var13 >> 16;
            }
        }
    } else {
        for (var12 = 0; var12 < var4; ++var12) {
            var13 = var8 + var10 * var12 / var3;
            var14 = var9 + var11 * var12 / var3;
            var1[var12] += data[var12] * var13 >> 16;
            var2[var12] += data[var12] * var14 >> 16;
        }
    }

    pitch = var6;
    volume = var7;
    return var5;
}