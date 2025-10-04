#include "Timer.h"

using namespace std;

Timer::Timer(float ticksPerSecond) {
    this->ticksPerSecond = ticksPerSecond;
    this->lastTime = Timer::nanoTime();
}

void Timer::advanceTime() {
    int64_t now = Timer::nanoTime();
    int64_t passedNs = now - this->lastTime;
    this->lastTime = now;
    if (passedNs < 0) {
        passedNs = 0;
    }
    if (passedNs > MAX_NS_PER_UPDATE) {
        passedNs = MAX_NS_PER_UPDATE;
    }
    this->fps = (float)NS_PER_SECOND / passedNs;
    this->passedTime += passedNs * this->timeScale * this->ticksPerSecond / 1.0E9f;
    this->ticks = this->passedTime;
    if (this->ticks > 100) {
        this->ticks = 100;
    }
    this->passedTime -= this->ticks;
    this->partialTicks = this->passedTime;
}

int64_t Timer::nanoTime() {
    return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count(); 
}