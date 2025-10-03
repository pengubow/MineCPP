#pragma once

#include "comm/ConnectionListener.h"
#include "comm/SocketConnection.h"

class MinecraftServer;

class Client : public ConnectionListener, public enable_shared_from_this<Client> {
public:
    const shared_ptr<SocketConnection> serverConnection;
private:
    MinecraftServer* server;
public:
    Client(MinecraftServer* server, shared_ptr<SocketConnection>& serverConnection);

    void command(uint8_t cmd, int32_t remaining, vector<uint8_t>& in) override;
    void handleException(const exception& e) override;
    void disconnect(); 
};