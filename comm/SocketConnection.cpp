#include <iostream>
#include <system_error>
#include <cstring>
#include "comm/SocketConnection.h"
#include "Timer.h"

SocketConnection::SocketConnection(const std::string& ip, int port)
    : io_context(),
    socketChannel(io_context),
    connected(false),
    lastRead(0),
    bytesRead(0),
    totalBytesWritten(0) {
    boost::system::error_code ec;

    // Resolve and connect (similar to Java's SocketChannel.open() + connect)
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(ip, std::to_string(port), ec);
    if (ec) {
        throw boost::system::system_error(ec);
    }

    socketChannel.open(tcp::v4(), ec);
    if (ec) throw boost::system::system_error(ec);

    boost::asio::connect(socketChannel, endpoints, ec);
    if (ec) throw boost::system::system_error(ec);

    // configure non-blocking like Java's configureBlocking(false)
    socketChannel.non_blocking(true, ec);
    // Java ignores any exception from configureBlocking(false) — we continue even if ec set

    lastRead = Timer::nanoTime() / 1000000;

    connected = true;
    readSize = 0;
    writeSize = 0;

    // mirror Java: get underlying socket from channel
    socket = &socketChannel;
}

void SocketConnection::setMaxBlocksPerIteration(int maxBlocksPerIteration) {
    this->maxBlocksPerIteration = maxBlocksPerIteration;
}

std::string SocketConnection::getIp() {
    if (socket) {
        boost::system::error_code ec;
        auto ep = socket->remote_endpoint(ec);
        if (!ec) {
            return ep.address().to_string();
        }
        // if remote_endpoint fails, fall through to empty string
    }
    return std::string();
}

SocketConnection::SocketConnection(tcp::socket socket)
    : io_context(),
    socketChannel(std::move(socket)),
    connected(true),
    lastRead(0),
    bytesRead(0),
    totalBytesWritten(0) {
    boost::system::error_code ec;
    socketChannel.non_blocking(true, ec);
    lastRead = Timer::nanoTime() / 1000000;

    // mirror Java behaviour: get socket from channel
    this->socket = &socketChannel;
}

std::vector<uint8_t>& SocketConnection::getBuffer() {
    return this->writeBuffer;
}

void SocketConnection::setConnectionListener(const std::shared_ptr<ConnectionListener>& listener) {
    this->connectionListener = listener;
}

bool SocketConnection::isConnected() {
    return this->connected;
}

void SocketConnection::disconnect() {
    this->connected = false;

    try {
        if (in) {
            // Java calls in.close(); in C++ we leave this as placeholder
            in = nullptr;
        }
    } catch (...) {
        // mimic Java catch and print
        std::cerr << "Exception while closing in\n";
    }

    try {
        if (out) {
            out = nullptr;
        }
    } catch (...) {
        std::cerr << "Exception while closing out\n";
    }

    try {
        if (socket) {
            boost::system::error_code ec;
            socket->close(ec);
            socket = nullptr;
        }
    } catch (...) {
        std::cerr << "Exception while closing socket\n";
    }
}

void SocketConnection::tick() {
    // Intentionally mirror Java: do NOT early-return on !connected.
    // On IO errors (other than would_block) throw an exception so caller behaves like Java's throws IOException.
    boost::system::error_code ec;

    // emulate Java's ByteBuffer.flip(); write then compact()
    if (writeSize > 0) {
        std::size_t bytes_written = socketChannel.write_some(
            boost::asio::buffer(writeBuffer.data(), writeSize),
            ec
        );

        if (ec && ec != boost::asio::error::would_block) {
            throw boost::system::system_error(ec);
        }

        if (!ec) {
            totalBytesWritten += static_cast<int>(bytes_written);

            if (bytes_written < writeSize) {
                std::memmove(writeBuffer.data(),
                             writeBuffer.data() + bytes_written,
                             writeSize - bytes_written);
                writeSize -= bytes_written;
            } else {
                writeSize = 0;
            }
        }
    }

    // emulate Java's read Buffer compact() / read then flip()
    std::size_t freeSpace = (readBuffer.size() > readSize) ? (readBuffer.size() - readSize) : 0;
    if (freeSpace > 0) {
        std::size_t bytes_read_now = socketChannel.read_some(
            boost::asio::buffer(readBuffer.data() + readSize, freeSpace),
            ec
        );

        if (ec && ec != boost::asio::error::would_block) {
            throw boost::system::system_error(ec);
        }

        if (!ec && bytes_read_now > 0) {
            readSize += bytes_read_now;
            bytesRead += static_cast<int>(bytes_read_now);
            lastRead = Timer::nanoTime() / 1000000;
        }
    }

    // emulate Java's flip(): process if data available
    if (readSize > 0 && connectionListener) {
        uint8_t cmd = readBuffer[0];
        // Assumes listener signature: void command(uint8_t, int, std::vector<uint8_t>&)
        connectionListener->command(cmd, static_cast<int>(readSize), readBuffer);

        // Java's code flips/consumes buffer; here we clear (same simple behavior as earlier)
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
