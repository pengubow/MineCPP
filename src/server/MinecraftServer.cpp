#include <thread>
#include <iostream>
#include "server/MinecraftServer.h"

MinecraftServer::MinecraftServer(vector<uint8_t>& ips, int32_t port) {
    socketServer = make_unique<SocketServer>(ips, port, this);
}

void MinecraftServer::clientConnected(shared_ptr<SocketConnection>& serverConnection) {
    shared_ptr<Client> client = make_shared<Client>(this, serverConnection);
    clientMap[serverConnection] = client;
    clients.push_back(client);
}

void MinecraftServer::disconnect(std::shared_ptr<Client>& client) {
    clientMap.erase(client->serverConnection);
    clients.erase(remove(clients.begin(), clients.end(), client), clients.end());
}

void MinecraftServer::clientException(shared_ptr<SocketConnection>& serverConnection, const exception& e) {
    shared_ptr<Client> client = clientMap[serverConnection];
    client->handleException(e);
}

void MinecraftServer::run() {
    while (true) {
        tick();
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

void MinecraftServer::tick() {
    try {
        socketServer->tick();
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
}

void MinecraftServer::main() {
    vector<uint8_t> byArray = vector<uint8_t>(4, 0);
    byArray[0] = 127;
    byArray[3] = 1;
    shared_ptr<MinecraftServer> server = make_shared<MinecraftServer>(byArray, 20801);
    thread t([server]() {
        server->run();
    });
}
