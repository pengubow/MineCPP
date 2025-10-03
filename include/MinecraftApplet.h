#pragma once

#include <thread>

using namespace std;

class MinecraftApplet {
public:
    thread t;
    void init();
    void startGameThread();
    void start();
    void stop();
    void destroy();
    void stopGameThread();
};