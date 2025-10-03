#include <chrono>
#include <system_error>
#include <cstring>
#include "comm/SocketConnection.h"
#include "Timer.h"

SocketConnection::SocketConnection(const std::string& ip, int port)
    : socketChannel(io_context) {
    boost::system::error_code ec;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(ip, std::to_string(port), ec);
    if (ec) throw boost::system::system_error(ec);

    socketChannel.open(tcp::v4(), ec);
    if (ec) throw boost::system::system_error(ec);

    boost::asio::connect(socketChannel, endpoints, ec);
    if (ec) throw boost::system::system_error(ec);

    socketChannel.non_blocking(true, ec);

    lastRead = Timer::nanoTime() / 1000000;

    connected = true;
    readSize = 0;
    writeSize = 0;
    socket = &socketChannel;
}

SocketConnection::SocketConnection(tcp::socket sock)
    : socketChannel(std::move(sock)) {
    boost::system::error_code ec;
    socketChannel.non_blocking(true, ec);

    lastRead = Timer::nanoTime() / 1000000;

    connected = true;
    readSize = 0;
    writeSize = 0;
    socket = &socketChannel;
}

void SocketConnection::setMaxBlocksPerIteration(int maxBlocksPerIteration) {
    this->maxBlocksPerIteration = maxBlocksPerIteration;
}

std::string SocketConnection::getIp() {
    if (socket) {
        boost::system::error_code ec;
        auto ep = socket->remote_endpoint(ec);
        if (!ec) return ep.address().to_string();
    }
    return std::string();
}

std::vector<uint8_t>& SocketConnection::getBuffer() {
    return this->writeBuffer;
}

void SocketConnection::setConnectionListener(ConnectionListener* listener) {
    this->connectionListener = listener;
}

bool SocketConnection::isConnected() {
    return this->connected;
}

void SocketConnection::disconnect() {
    this->connected = false;
    try { if (in) in = nullptr; } catch(...) {}
    try { if (out) out = nullptr; } catch(...) {}
    try {
        if (socket) {
            boost::system::error_code ec;
            socket->close(ec);
            socket = nullptr;
        }
    } catch(...) {}
}

void SocketConnection::tick() {
    boost::system::error_code ec;
    if (writeSize > 0) {
        std::size_t bytes_written = socketChannel.write_some(
            boost::asio::buffer(writeBuffer.data(), writeSize), ec);
        if (ec && ec != boost::asio::error::would_block) throw boost::system::system_error(ec);
        if (!ec) {
            totalBytesWritten += static_cast<int>(bytes_written);
            if (bytes_written < writeSize) {
                std::memmove(writeBuffer.data(), writeBuffer.data() + bytes_written, writeSize - bytes_written);
                writeSize -= bytes_written;
            } else {
                writeSize = 0;
            }
        }
    }
    std::size_t freeSpace = (readBuffer.size() > readSize) ? (readBuffer.size() - readSize) : 0;
    if (freeSpace > 0) {
        std::size_t bytes_read_now = socketChannel.read_some(
            boost::asio::buffer(readBuffer.data() + readSize, freeSpace), ec);
        if (ec && ec != boost::asio::error::would_block) throw boost::system::system_error(ec);
        if (!ec && bytes_read_now > 0) {
            readSize += bytes_read_now;
            bytesRead += static_cast<int>(bytes_read_now);
            lastRead = Timer::nanoTime() / 1000000;
        }
    }
    if (readSize > 0 && connectionListener) {
        uint8_t cmd = readBuffer[0];
        connectionListener->command(cmd, static_cast<int>(readSize), readBuffer);
        readSize = 0;
    }
}

int SocketConnection::getSentBytes() {
    return this->totalBytesWritten;
}

int SocketConnection::getReadBytes() {
    return this->bytesRead;
}

void SocketConnection::clearSentBytes() {
    this->totalBytesWritten = 0;
}

void SocketConnection::clearReadBytes() {
    this->bytesRead = 0;
}