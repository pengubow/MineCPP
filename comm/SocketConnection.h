#pragma once

#include <boost/asio.hpp>
#include "comm/ConnectionListener.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <chrono>

using boost::asio::ip::tcp;

class SocketConnection {
public:
    static const int BUFFER_SIZE = 131068;
private:
    bool connected;
    boost::asio::io_context io_context;
    tcp::socket socketChannel;
public:
    vector<uint8_t> readBuffer = vector<uint8_t>(BUFFER_SIZE);
    vector<uint8_t> writeBuffer = vector<uint8_t>(BUFFER_SIZE);
protected:
    int64_t lastRead;
private:
    shared_ptr<ConnectionListener> connectionListener;
    int32_t bytesRead;
    int32_t totalBytesWritten;
    int32_t maxBlocksPerIteration = 3;
    tcp::socket* socket = nullptr;
    istream* in = nullptr;
    ostream* out = nullptr;
    int32_t readSize = 0;
    int32_t writeSize = 0;
public:
    SocketConnection(const string& ip, int port);

    void setMaxBlocksPerIteration(int maxBlocksPerIteration);
    string getIp();

    SocketConnection(tcp::socket socket);

    vector<uint8_t>& getBuffer();
    void setConnectionListener(const shared_ptr<ConnectionListener>& listener);
    bool isConnected();
    void disconnect();
    void tick();
    int getSentBytes();
    int getReadBytes();
    void clearSentBytes();
    void clearReadBytes();
};