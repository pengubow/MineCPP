#include "net/ConnectionManager.h"
#include "comm/SocketConnection.h"
#include "net/Packet.h"
#include "Minecraft.h"

ConnectionManager::ConnectionManager(shared_ptr<Minecraft>& var1, const string& var2, int var3, const string& var4)
    : connection(nullptr), minecraft(var1) {
    try {
        this->connection = new SocketConnection(var2, var3);
        SocketConnection* var5 = this->connection;
        var5->manager = this;
        this->connection->sendPacket(Packet::LOGIN, {var4});
        this->minecraft = var1;
    } catch (const exception& e) {
        throw;
    }
}