#include <system_error>
#include "comm/SocketServer.h"

SocketServer::SocketServer(const std::vector<uint8_t>& ips, int32_t port, ServerListener* serverListener)
: io_context(), acceptor(io_context), serverListener(serverListener)
{
    if (ips.size() != 4) throw std::invalid_argument("invalid ip");
    boost::asio::ip::address_v4::bytes_type b = { ips[0], ips[1], ips[2], ips[3] };
    boost::asio::ip::address addr = boost::asio::ip::address_v4(b);
    boost::system::error_code ec;
    tcp::endpoint endpoint(addr, static_cast<unsigned short>(port));
    acceptor.open(endpoint.protocol(), ec);
    if (ec) throw boost::system::system_error(ec);
    acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    acceptor.bind(endpoint, ec);
    if (ec) throw boost::system::system_error(ec);
    acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) throw boost::system::system_error(ec);
    acceptor.non_blocking(true, ec);
}

void SocketServer::tick() {
    while (true) {
        boost::system::error_code ec;
        tcp::socket sock(io_context);
        acceptor.accept(sock, ec);
        if (ec == boost::asio::error::would_block || ec == boost::asio::error::try_again) break;
        if (ec) throw boost::system::system_error(ec);
        try {
            sock.non_blocking(true, ec);
            shared_ptr<SocketConnection> conn = std::make_shared<SocketConnection>(std::move(sock));
            if (serverListener) serverListener->clientConnected(conn);
            connections.push_back(std::move(conn));
        } catch (...) {
            throw;
        }
    }
    for (std::size_t i = 0; i < connections.size(); ++i) {
        shared_ptr<SocketConnection> socketConnection = connections[i];
        if (!socketConnection->isConnected()) {
            socketConnection->disconnect();
            connections.erase(connections.begin() + static_cast<std::ptrdiff_t>(i));
            --i;
        } else {
            try {
                socketConnection->tick();
            } catch (const std::exception& e) {
                socketConnection->disconnect();
                if (serverListener) serverListener->clientException(socketConnection, e);
            }
        }
    }
}