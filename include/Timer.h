#pragma once

#include <cstdint>
#include <chrono>

using namespace std;

class Timer {
public:
    float ticksPerSecond;
    double lastHRTime;
    int32_t ticks;
    float a;
    float timeScale = 1.0f;
    float fps = 0.0f;
    int64_t lastSyncSysClock;
    int64_t lastSyncHRClock;
    double timeSyncAdjustment = 1.0;

    Timer(float ticksPerSecond);

    static int64_t nanoTime();
};
