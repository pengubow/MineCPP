#include "Timer.h"

using namespace std;

Timer::Timer(float ticksPerSecond) {
    this->ticksPerSecond = ticksPerSecond;
    this->lastTime = Timer::nanoTime();
}

int64_t Timer::nanoTime() {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count(); 
}