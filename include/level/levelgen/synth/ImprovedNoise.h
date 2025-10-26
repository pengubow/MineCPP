#pragma once

#include <vector>
#include <stdint.h>
#include "level/levelgen/synth/Synth.h"
#include "Util/Random.h"

using namespace std;

class ImprovedNoise : public Synth {
    vector<int32_t> p;
public:
    ImprovedNoise();
    ImprovedNoise(Random& random);
private:
    static double fade(double var0);
    static double lerp(double var0, double var2, double var4);
    static double grad(int32_t var0, double var1, double var3, double var5);
public:
    double getValue(double var1, double var3) override;
};