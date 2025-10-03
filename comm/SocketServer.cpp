#include "comm/SocketServer.h"

#include <iostream>
#include <system_error>

SocketServer::SocketServer(const std::vector<uint8_t>& ips, int port, const std::shared_ptr<ServerListener>& serverListener)
: io_context(),
  acceptor(io_context),
  serverListener(serverListener)
{
    if (ips.size() != 4) {
        throw std::invalid_argument("Only IPv4 4-byte address supported in this constructor");
    }

    // build address from bytes (mirrors InetAddress.getByAddress)
    boost::asio::ip::address_v4::bytes_type b = {
        ips[0], ips[1], ips[2], ips[3]
    };
    boost::asio::ip::address addr = boost::asio::ip::address_v4(b);

    boost::system::error_code ec;

    tcp::endpoint endpoint(addr, static_cast<unsigned short>(port));

    acceptor.open(endpoint.protocol(), ec);
    if (ec) throw boost::system::system_error(ec);

    // allow address reuse (common server behaviour)
    acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) {
        // continue but warn
        logger << "Warning: set_option(reuse_address) failed: " << ec.message() << '\n';
    }

    acceptor.bind(endpoint, ec);
    if (ec) throw boost::system::system_error(ec);

    acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) throw boost::system::system_error(ec);

    // configure non-blocking to mirror Java's configureBlocking(false)
    acceptor.non_blocking(true, ec);
    if (ec) {
        // Java ignores configureBlocking failure; we warn but continue
        logger << "Warning: acceptor.non_blocking failed: " << ec.message() << '\n';
    }
}

void SocketServer::tick() {
    // Accept loop: mirror Java's while ((socketChannel = ssc.accept()) != null)
    while (true) {
        boost::system::error_code ec;
        tcp::socket socket(io_context);

        // non-blocking accept: returns would_block when no pending connection
        acceptor.accept(socket, ec);

        if (ec == boost::asio::error::would_block || ec == boost::asio::error::try_again) {
            // no more pending connections — mirror Java's null result
            break;
        }

        if (ec) {
            // real error — throw (Java's constructor/tick throws IOException)
            throw boost::system::system_error(ec);
        }

        // Got a new connection
        try {
            // log remote address (like Java's logger.log(Level.INFO, socketChannel.socket().getRemoteSocketAddress() + " connected"))
            boost::system::error_code ep_ec;
            auto remote = socket.remote_endpoint(ep_ec);
            if (!ep_ec) {
                logger << remote.address().to_string() << ":" << remote.port() << " connected\n";
            } else {
                logger << "Unknown remote connected\n";
            }

            // set non-blocking on the accepted socket (like configureBlocking(false))
            socket.non_blocking(true, ec);
            // proceed even if non_blocking setting failed (matches Java's behavior)

            // create SocketConnection from the accepted socket (move)
            auto conn = std::make_unique<SocketConnection>(std::move(socket));
            // call serverListener.clientConnected(conn) — pass pointer for 1:1 feel
            if (serverListener) {
                serverListener->clientConnected(conn.get());
            }

            // add to connections list
            connections.push_back(std::move(conn));
        }
        catch (const std::exception& e) {
            // In Java you call socketChannel.close() and rethrow the exception.
            // Here the local socket will be closed when it goes out of scope; rethrow to let caller handle.
            throw;
        }
    }

    // Iterate connections the same way Java does (index loop, removing when disconnected)
    for (std::size_t i = 0; i < connections.size(); ++i) {
        SocketConnection* socketConnection = connections[i].get();

        if (!socketConnection->isConnected()) {
            socketConnection->disconnect();
            // remove and step index back (mirror Java's remove(i--) pattern)
            connections.erase(connections.begin() + static_cast<std::ptrdiff_t>(i));
            --i; // adjust because container shrunk
        } else {
            try {
                socketConnection->tick();
            } catch (const std::exception& e) {
                socketConnection->disconnect();
                if (serverListener) {
                    serverListener->clientException(socketConnection, e);
                }
            }
        }
    }
}
