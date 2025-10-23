#pragma once

#ifdef _WIN32 // god damn it windows sucks
    #ifndef _WINSOCKAPI_
        #define _WINSOCKAPI_
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>

using namespace std;

class Minecraft;
class SocketConnection;
class NetworkPlayer;

class ConnectionManager {
public:
    vector<uint8_t> levelBuffer;
    SocketConnection* connection;
    weak_ptr<Minecraft> minecraft;
    unordered_map<uint8_t, shared_ptr<NetworkPlayer>> players;

    ConnectionManager(shared_ptr<Minecraft>& var1, const string& var2, int var3, const string& var4);

    void sendBlockChange(int16_t var1, int16_t var2, int16_t var3, int8_t var4, int8_t var5);
};