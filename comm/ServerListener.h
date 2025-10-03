#include <exception>
#include "comm/SocketConnection.h"

class ServerListener {
public:
    virtual void clientConnected(SocketConnection* var1) = 0;
    virtual void clientException(SocketConnection* var1, const std::exception& var2) = 0;
};