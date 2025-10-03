#include <boost/asio.hpp>
#include "comm/ServerListener.h"
#include "comm/SocketConnection.h"

#include <vector>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

class SocketServer {
private:
    // acceptor — equivalent to Java's ServerSocketChannel ssc
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;

    // listener and connection list
    std::shared_ptr<ServerListener> serverListener;
    std::vector<std::unique_ptr<SocketConnection>> connections;

    // simple logger placeholder
    static inline std::ostream& logger = std::clog;

public:
    // ips should be 4 bytes (IPv4) in the same style as Java's byte[] ips
    SocketServer(const std::vector<uint8_t>& ips, int port, const std::shared_ptr<ServerListener>& serverListener);

    // tick() mirrors Java: accept pending sockets and iterate connections
    void tick();

    // non-copyable (sockets / acceptor shouldn't be copied)
    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;
    SocketServer(SocketServer&&) = delete;
    SocketServer& operator=(SocketServer&&) = delete;
};