#include "Timer.h"

using namespace std;

Timer::Timer(float ticksPerSecond) {
    this->ticksPerSecond = ticksPerSecond;
    this->lastSyncSysClock = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    this->lastSyncHRClock = Timer::nanoTime() / 1000000;
}

int64_t Timer::nanoTime() {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count(); 
}