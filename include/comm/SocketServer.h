#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include "comm/ServerListener.h"
#include "SocketConnection.h"

using boost::asio::ip::tcp;

class SocketServer {
private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;
    ServerListener* serverListener;
    vector<shared_ptr<SocketConnection>> connections;

public:
    SocketServer(const vector<uint8_t>& ips, int32_t port, ServerListener* serverListener);
    void tick();
};