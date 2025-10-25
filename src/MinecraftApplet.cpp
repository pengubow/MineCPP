
#include <stdint.h>
#include <map>
#include <future>
#include "Minecraft.h"
#include "MinecraftApplet.h"
#include "User.h"
#include "net/Packet.h"

shared_ptr<Minecraft> MinecraftApplet::minecraft;

map<string, string> parseArgs(int argc, char* argv[]) {
    map<string, string> args;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            string key = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args[key] = argv[++i];
            }
        }
    }
    return args;
}

void MinecraftApplet::init(int argc, char* argv[]) {
    MinecraftApplet::minecraft = make_shared<Minecraft>(854, 480, false);
    map<string, string> args = parseArgs(argc, argv);
    if (args.count("username") && args.count("sessionId")) {
        minecraft->user = make_shared<User>(args["username"], args["sessionId"]);
        if (args.count("mppass")) {
            minecraft->user->mpPass = args["mppass"];
        }
    }

    if (args.count("mapUser") && args.count("mapId")) {
        minecraft->loadMapUser = args["mapUser"];
        minecraft->loadMapID = stoi(args["mapId"]);
    }

    if (args.count("server") && args.count("port")) {
        minecraft->server = args["server"];
        minecraft->port = stoi(args["port"]);
    }

    minecraft->minecraftUri = "www.betacraft.uk"; // for loading worlds
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
    if (t.joinable()) {
        minecraft->running = false;
        
        try {
            auto future = async(launch::async, [this]() {
                if (t.joinable()) {
                    t.join();
                }
            });
            
            auto status = future.wait_for(chrono::milliseconds(1000));
        } catch (const exception& e) {
            try {
                minecraft->destroy();
            } catch (const exception& e2) {
                cerr << e2.what() << endl;
            }
        }
        this->t = thread();
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        string help = argv[1];
        if (help == "-h" || help == "--help") {
            cout << "MineCPP" << endl;
            cout << "Options:" << endl;
            cout << "  --username <name>      Set player username" << endl;
            cout << "  --sessionId <id>       Set session ID" << endl;
            cout << "  --mppass <pass>        Sets multiplayer pass" << endl;
            cout << "  --mapUser <user>       Set map owner username" << endl;
            cout << "  --mapId <id>           Set map ID" << endl;
            cout << "  --server <ip>          Set server host" << endl;
            cout << "  --port <port>          Set server port" << endl;
            cout << "  -h, --help             Display this help message" << endl;
            return 0;
        }
    }
    
    MinecraftApplet applet;
    applet.init(argc, argv);
    applet.t.join();
    return 0;
}