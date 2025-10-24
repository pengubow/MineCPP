#pragma once

#include <memory>
#include "level/levelgen/synth/ImprovedNoise.h"

class PerlinNoise : public Synth {
    vector<shared_ptr<ImprovedNoise>> noiseLevels = vector<shared_ptr<ImprovedNoise>>(8);
    int32_t levels = 8;
public:
    PerlinNoise(Random random, int32_t var1);

    double getValue(double var1, double var2) override;
};