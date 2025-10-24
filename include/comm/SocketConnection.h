#include <string>
#include <vector>
#include <cstdint>
#include <variant>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <atomic>
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include <intrin.h>
    #pragma comment(lib, "ws2_32.lib")

    typedef SOCKET SocketHandle;
    #define INVALID_SOCKET_HANDLE INVALID_SOCKET
    #define htobe64(x) _byteswap_uint64(x)
    #define be64toh(x) _byteswap_uint64(x)
    #define close closesocket
#elif defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <libkern/OSByteOrder.h>
    
    typedef int SocketHandle;
    #define INVALID_SOCKET_HANDLE -1
    #define htobe64(x) OSSwapHostToBigInt64(x)
    #define be64toh(x) OSSwapBigToHostInt64(x)
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    
    typedef int SocketHandle;
    #define INVALID_SOCKET_HANDLE -1
#endif

using namespace std;

class Packet;
class ConnectionManager;

using PacketValue = variant<
    int64_t,
    int32_t,
    int16_t,
    int8_t,
    double,
    float,
    string,
    vector<uint8_t>
>;

class SocketConnection {
public:
    bool connected = false;
private:
    SocketHandle socketHandle;
public:
    vector<uint8_t> readBuffer;
    vector<uint8_t> writeBuffer;
    size_t readPos;
    size_t writePos;
    ConnectionManager* manager;
    bool initialized;
private:
    uint8_t stringPacket[64];
public:
    SocketConnection(const string& var1, int var2);

    void disconnect();
    void processDataFunc();
    void sendPacket(Packet* var1, const vector<PacketValue>& var2);
private:
    PacketValue read(Packet* packet, size_t fieldIndex);
};