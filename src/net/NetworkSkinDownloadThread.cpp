#include "net/NetworkSkinDownloadThread.h"
#include "Minecraft.h"

NetworkSkinDownloadThread::NetworkSkinDownloadThread(NetworkPlayer* player)
    : player(player) {}

void NetworkSkinDownloadThread::run() {
    try {
        shared_ptr<Minecraft> minecraft = player->minecraft.lock();
        if (!minecraft) {
            return;
        }
        httplib::Client cli(minecraft->minecraftUri, 80);
        cli.set_connection_timeout(10, 0);
        
        string path = "/skin/" + player->name + ".png";
        
        auto res = cli.Get(path);

        if (res && res->status != 404) {
            cout << "Loading texture for " << player->name << endl;
            int32_t width, height, channels;
            uint8_t* data = stbi_load_from_memory((uint8_t*)res->body.data(), res->body.size(), &width, &height, &channels, 4);
            if (!data) {
                cout << "Failed to load texture for " << player->name << endl;
                return;
            }
            
            player->skinWidth = width;
            player->skinHeight = height;
            player->newTexture = data;
            return;
        }

        cout << "Failed to load texture for " << player->name << endl;
        return;
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
}

void NetworkSkinDownloadThread::start() {
    thread([self = shared_from_this()]() {
        self->run();
    }).detach();
}