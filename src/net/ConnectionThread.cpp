#include "net/ConnectionThread.h"
#include "comm/SocketConnection.h"
#include "net/Packet.h"
#include "Minecraft.h"
#include "gui/ErrorScreen.h"

ConnectionThread::ConnectionThread(ConnectionManager* connectionManager, const string& ip, int32_t port, const string& username, const string& mpPass, shared_ptr<Minecraft> minecraft) 
    : connectionManager(connectionManager), ip(ip), port(port),
    username(username), mpPass(mpPass), minecraft(minecraft) {}

void ConnectionThread::run() {
    try {
        SocketConnection* connection = new SocketConnection(ip, port);
        connectionManager->connection = connection;
        connection->manager = connectionManager;
        connection->sendPacket(Packet::LOGIN, {(int8_t)6, username, mpPass, (int8_t)0});
        connectionManager->processData = true;
    } catch (const exception& e) {
        minecraft->hideGui = false;
        minecraft->connectionManager = nullptr;
        minecraft->setScreen(make_shared<ErrorScreen>("Failed to connect", "You failed to connect to the server. It's probably down!"));
        connectionManager->processData = false;
    }
}

void ConnectionThread::start() {
    thread([this]() {
        run();
    }).detach();
}