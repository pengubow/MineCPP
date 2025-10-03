#pragma once

#include <exception>
#include "comm/SocketConnection.h"

class ServerListener {
public:
    virtual void clientConnected(shared_ptr<SocketConnection>& var1) = 0;
    virtual void clientException(shared_ptr<SocketConnection>& var1, const exception& var2) = 0;
};