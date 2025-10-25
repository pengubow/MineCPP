#include <memory>
#include "net/NetworkPlayer.h"
#include "Util/stb_image.h"
#include "Util/httplib.h"

using namespace std;

class NetworkPlayer;

class NetworkSkinDownloadThread : public enable_shared_from_this<NetworkSkinDownloadThread> {
    NetworkPlayer* player;
public:
    NetworkSkinDownloadThread(NetworkPlayer* player);

    void run();
    void start();
};