#pragma once

#include <cstdint>
#include <chrono>

using namespace std;

class Timer {
public:
    float ticksPerSecond;
    int64_t lastTime;
    int32_t ticks;
    float partialTicks;
    float timeScale = 1.0f;
    float fps = 0.0f;

    Timer(float ticksPerSecond);

    static int64_t nanoTime();
};
