#include "level/levelgen/synth/Distort.h"

Distort::Distort(shared_ptr<Synth> source, shared_ptr<Synth> distort)
    : source(source), distort(distort) {}

double Distort::getValue(double var1, double var2) {
    return source->getValue(var1 + distort->getValue(var1, var2), var2);
}