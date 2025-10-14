#pragma once

#include <thread>

using namespace std;

class Minecraft;

class MinecraftApplet {
private:
    static shared_ptr<Minecraft> minecraft;
public:
    thread t;
    void init(int argc, char** argv);
    void startGameThread();
    void start();
    void stop();
    void destroy();
    void stopGameThread();
};