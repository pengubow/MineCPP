#pragma once

#include <cstdint>
#include <chrono>
#include <stdexcept>

using namespace std;

class Random {
private:
    static constexpr uint64_t multiplier = 25214903917;
    static constexpr uint64_t addend = 11;
    static constexpr uint64_t mask = 281474976710655;
    uint64_t seed;
    static Random* mathRandom;
public:
    Random();
    Random(uint64_t seed);

    int32_t next(int32_t bits);
    void setSeed(uint64_t s);
    int32_t nextInt();
    int32_t nextInt(int32_t max);
    int32_t nextInt(int32_t min, int32_t max);
    float nextFloat();
    float nextFloat(float max);
    float nextFloat(float min, float max);
    double nextDouble();
    double nextDouble(double max);
    double nextDouble(double min, double max);
    bool nextBoolean();
    void nextBytes(uint8_t* bytes, int32_t length);
    int64_t nextLong();
    static double random();
};