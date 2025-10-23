#pragma once

#include <memory>
#include "level/levelgen/synth/Synth.h"
#include "level/levelgen/synth/PerlinNoise.h"

using namespace std;

class Distort : public Synth {
    shared_ptr<Synth> source;
    shared_ptr<Synth> distort;
public:
    Distort(shared_ptr<Synth> source, shared_ptr<Synth> distort);

    double getValue(double var1, double var2) override;
};