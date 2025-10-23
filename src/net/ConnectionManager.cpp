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
        this->connection->sendPacket(Packet::LOGIN, {(int8_t)3, var4, "--"});
        this->minecraft = var1;
        var1->beginLevelLoading("Connecting..");
        var1->hideGui = true;
    } catch (const exception& e) {
        throw;
    }
}

void ConnectionManager::sendBlockChange(int16_t var1, int16_t var2, int16_t var3, int8_t var4, int8_t var5) {
    connection->sendPacket(Packet::PLACE_OR_REMOVE_TILE, {var1, var2, var3, var4, var5});
}