#pragma once

#include <cstdint>
#include <chrono>

using namespace std;

class Timer {
private:
    static const int64_t NS_PER_SECOND = 1000000000;
    static const int64_t MAX_NS_PER_UPDATE = 1000000000;
    static const int32_t MAX_TICKS_PER_UPDATE = 100;
    float ticksPerSecond;
    int64_t lastTime;
public:
    int32_t ticks;
    float partialTicks;
    float timeScale = 1.0f;
    float fps = 0.0f;
    float passedTime = 0.0f;

    Timer(float ticksPerSecond);

    void advanceTime();
    static int64_t nanoTime();
};
