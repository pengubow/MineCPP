#include <memory>
#include "comm/ServerListener.h"
#include "comm/SocketServer.h"
#include "server/Client.h"

class MinecraftServer : public ServerListener {
private:
    unique_ptr<SocketServer> socketServer;
    unordered_map<shared_ptr<SocketConnection>, shared_ptr<Client>> clientMap;
    vector<shared_ptr<Client>> clients;
public:
    MinecraftServer(vector<uint8_t>& ips, int32_t port);

    void clientConnected(shared_ptr<SocketConnection>& serverConnection) override;
    void disconnect(shared_ptr<Client>& client);
    void clientException(shared_ptr<SocketConnection>& serverConnection, const exception& e) override;
    void run();
private:
    void tick();
public:
    static void main();
};