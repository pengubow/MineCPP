#include "net/ConnectionManager.h"
#include "comm/SocketConnection.h"
#include "net/Packet.h"
#include "Minecraft.h"
#include "gui/ErrorScreen.h"
#include "net/ConnectionThread.h"
#include "Minecraft.h"

ConnectionManager::ConnectionManager(shared_ptr<Minecraft>& var1, const string& var2, int var3, const string& var4, const string& mpPass)
    : minecraft(var1), connection(nullptr) {
    var1->hideGui = true;
    thread([this, var2, var3, var4, mpPass, var1]() {
        ConnectionThread* ct = new ConnectionThread(this, var2, var3, var4, mpPass, var1);
        ct->run();
    }).detach();
}

void ConnectionManager::sendBlockChange(int16_t var1, int16_t var2, int16_t var3, int8_t var4, int8_t var5) {
    connection->sendPacket(Packet::PLACE_OR_REMOVE_TILE, {var1, var2, var3, var4, var5});
}

void ConnectionManager::disconnect(const exception& e) {
    connection->disconnect();
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    minecraft->setScreen(make_shared<ErrorScreen>("Disconnected!", e.what()));
}

bool ConnectionManager::isConnected() {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return false;
    }

    if (connection) {
        if (connection->connected) {
            return true;
        }
    }

    return false;
}