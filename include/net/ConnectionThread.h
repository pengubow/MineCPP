#include "ConnectionManager.h"

class ConnectionThread {
    string ip;
    int32_t port;
    string username;
    string mpPass;
    shared_ptr<Minecraft> minecraft;
    ConnectionManager* connectionManager;
public:
    ConnectionThread(ConnectionManager* connectionManager, const string& ip, int32_t port, const string& username, const string& mpPass, shared_ptr<Minecraft> minecraft);

    void run();
};