
#include <stdint.h>
#include "Minecraft.h"
#include "MinecraftApplet.h"
#include "User.h"

shared_ptr<Minecraft> MinecraftApplet::minecraft;

void MinecraftApplet::init(int argc, char* argv[]) {
    MinecraftApplet::minecraft = make_shared<Minecraft>(854, 480, false);
    minecraft->minecraftUri = "localhost:25565";

    if (argc > 2) {
        string name = argv[1];
        string sessionId = argv[2];
        minecraft->user = make_shared<User>(name, sessionId);
    }

    if (argc > 4) {
        minecraft->loadMapUser = argv[3];
        minecraft->loadMapID = stoi(argv[4]);
    }
    
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

int main(int argc, char** argv) {
    MinecraftApplet applet;
    applet.init(argc, argv);
    applet.t.join();
    return 0;
}

