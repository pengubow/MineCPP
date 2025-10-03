#include "server/Client.h"
#include "server/MinecraftServer.h"

Client::Client(MinecraftServer* server, shared_ptr<SocketConnection>& serverConnection) 
    : server(server), serverConnection(serverConnection) {
        serverConnection->setConnectionListener(this);
}

void Client::command(uint8_t cmd, int32_t remaining, vector<uint8_t>& in) {}

void Client::handleException(const exception& e) {
    disconnect();
}

void Client::disconnect() {
    shared_ptr<Client> client = shared_from_this();
    server->disconnect(client);
}