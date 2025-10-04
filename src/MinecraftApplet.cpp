
#include <stdint.h>
#include "Minecraft.h"
#include "MinecraftApplet.h"

Minecraft* minecraft = new Minecraft(640, 480, false);

void MinecraftApplet::init() {
    minecraft->applet = this;
    startGameThread();
}

void MinecraftApplet::startGameThread() {
    this->t = thread([this]() {
        minecraft->run();
    });
}

void MinecraftApplet::start() {
    minecraft->pause = false;
}

void MinecraftApplet::stop() {
    minecraft->pause = true;
}

void MinecraftApplet::destroy() {
    stopGameThread();
}

void MinecraftApplet::stopGameThread() {
    minecraft->stop();
}

int main() {
    MinecraftApplet applet;
    applet.init();
    applet.t.join();
    return 0;
}

