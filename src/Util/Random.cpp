#include "Util/Random.h"

Random* Random::mathRandom = nullptr;

Random::Random() {
    uint64_t nanoTime = chrono::high_resolution_clock::now().time_since_epoch().count();
    setSeed(nanoTime);
}

Random::Random(uint64_t initialSeed) {
    setSeed(initialSeed);
}

int32_t Random::next(int32_t bits) {
    seed = (seed * multiplier + addend) & mask;
    return (int32_t)(seed >> (48 - bits));
}

void Random::setSeed(uint64_t s) {
    seed = (s ^ multiplier) & mask;
}

int32_t Random::nextInt() {
    return next(32);
}

int32_t Random::nextInt(int32_t max) {
    if (max <= 0) {
        throw invalid_argument("nextInt: max must be positive");
    }

    if ((max & (max - 1)) == 0) {
        return (int32_t)(((int64_t)next(31) * max) >> 31);
    }

    int32_t bits, val;
    do {
        bits = next(31);
        val = bits % max;
    } while (bits - val + (max - 1) < 0);
    return val;
}

int32_t Random::nextInt(int32_t min, int32_t max) {
    if (max <= min) {
        throw invalid_argument("nextInt: max must be greater than min");
    }
    return min + nextInt(max - min);
}

float Random::nextFloat() {
    return next(24) / 16777216.0f;
}

float Random::nextFloat(float max) {
    if (max <= 0.0f) {
        throw invalid_argument("nextFloat: max must be positive");
    }
    return nextFloat() * max;
}

float Random::nextFloat(float min, float max) {
    if (max <= min) {
        throw invalid_argument("nextFloat: max must be greater than min");
    }
    return min + nextFloat() * (max - min);
}

double Random::nextDouble() {
    uint64_t a = (uint64_t)next(26);
    uint64_t b = (uint64_t)next(27);
    uint64_t combined = (a << 27) + b;
    return (double)combined / 9007199254740992.0;
}

double Random::nextDouble(double max) {
    if (max <= 0.0) {
        throw invalid_argument("nextDouble: max must be positive");
    }
    return nextDouble() * max;
}

double Random::nextDouble(double min, double max) {
    if (max <= min) {
        throw invalid_argument("nextDouble: max must be greater than min");
    }
    return min + nextDouble() * (max - min);
}

bool Random::nextBoolean() {
    return next(1) != 0;
}

void Random::nextBytes(uint8_t* bytes, int32_t length) {
    for (int32_t i = 0; i < length; i++) {
        bytes[i] = (uint8_t)next(8);
    }
}

int64_t Random::nextLong() {
    return ((int64_t)next(32) << 32) + next(32);
}

double Random::random() {
    if (mathRandom == nullptr) {
        mathRandom = new Random();
    }
    return mathRandom->nextDouble();
}