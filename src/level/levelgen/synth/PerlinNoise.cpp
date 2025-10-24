#include "level/levelgen/synth/PerlinNoise.h"

PerlinNoise::PerlinNoise(Random random, int32_t var1) {
    for (var1 = 0; var1 < 8; var1++) {
        noiseLevels[var1] = make_shared<ImprovedNoise>(random);
    }
}

double PerlinNoise::getValue(double var1, double var2) {
    double var5 = 0.0;
    double var7 = 1.0;

    for(int32_t var9 = 0; var9 < levels; ++var9) {
        var5 += noiseLevels[var9]->getValue(var1 / var7, var2 / var7) * var7;
        var7 *= 2.0;
    }

    return var5;
}