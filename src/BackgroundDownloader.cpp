#include <iostream>
#include <fstream>
#include <sstream>
#include "Util/httplib.h"
#include "BackgroundDownloader.h"
#include "Minecraft.h"
#include "sound/SoundManager.h"

BackgroundDownloader::BackgroundDownloader(const fs::path& workDir, shared_ptr<Minecraft>& minecraft)
    : minecraft(minecraft) {
    
    resourcesFolder = workDir;
    if (!fs::exists(resourcesFolder)) {
        fs::create_directories(resourcesFolder);
    }
    if (!fs::exists(resourcesFolder)) {
        throw runtime_error("The working directory could not be created: " + resourcesFolder.string());
    }

    thread t(&BackgroundDownloader::run, this);
    t.detach();
}

void BackgroundDownloader::stop() {
    closing = true;
}

void BackgroundDownloader::downloadResource(const string& url, const fs::path& file) {
    size_t pathStart = url.find('/');
    
    string host;
    string path;
    
    if (pathStart == string::npos) {
        host = url;
        path = "/";
    } else {
        host = url.substr(0, pathStart);
        path = url.substr(pathStart);
    }
    
    httplib::Client cli(host, 80);
    cli.set_connection_timeout(50, 0);
    cli.set_read_timeout(10, 0);

    auto res = cli.Get(path);
    if (!res || res->status != 200) {
        cerr << "Failed to download: " << url << " (status: " << (res ? res->status : -1) << ")" << endl;
        return;
    }

    if (res->body.empty()) {
        cout << "empty body" << endl;
        return;
    }

    ofstream outfile(file, ios::binary);
    if (!outfile) {
        cout << "Failed to open file: " << file << endl;
        return;
    }

    outfile.write(res->body.data(), res->body.size());
    outfile.close();
}

void BackgroundDownloader::run() {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    try {
        httplib::Client cli(minecraft->minecraftUri, 80);
        cli.set_connection_timeout(5, 0);
        cli.set_read_timeout(5, 0);
        auto res = cli.Get("/resources/");
        if (!res || res->status != 200) {
            cerr << "Failed to download resource list" << endl;
            return;
        }

        istringstream stream(res->body);
        string line;
        vector<string> resources;

        while (getline(stream, line)) {
            if (!line.empty()) {
                resources.push_back(line);
            }
        }

        for (const string& resource : resources) {
            if (closing) {
                return;
            }

            try {
                size_t commaPos1 = resource.find(',');
                size_t commaPos2 = resource.find(',', commaPos1 + 1);

                if (commaPos1 == string::npos || commaPos2 == string::npos) {
                    continue;
                }

                string name = resource.substr(0, commaPos1);

                int size = stoi(resource.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1));

                const string prefix = "resources/sounds/";
                if (name.substr(0, prefix.length()) == prefix) {
                    name = name.substr(prefix.length());
                }
                
                fs::path resourcePath = resourcesFolder / name;

                if (!fs::exists(resourcePath) || (int)fs::file_size(resourcePath) != size) {
                    fs::create_directories(resourcePath.parent_path());
                    downloadResource(minecraft->minecraftUri + "/resources/" + name, resourcePath);

                    if (closing) {
                        return;
                    }
                }

                size_t slashPos = name.find('/');
                if (slashPos == string::npos) {
                    continue;
                }

                string category = name.substr(0, slashPos);
                string filename = name.substr(slashPos + 1);

                shared_ptr<Minecraft> minecraft = this->minecraft.lock();
                if (!minecraft) {
                    return;
                }

                if (category == "sound") {
                    minecraft->soundManager->registerSound(resourcePath.string(), filename);
                } else if (category == "music") {
                    minecraft->soundManager->registerMusic(filename, resourcePath.string());
                }

            } catch (const exception& e) {
                cerr << "Error processing resource: " << e.what() << endl;
            }

            if (closing) {
                return;
            }
        }

    } catch (const exception& e) {
        cerr << "BackgroundDownloader error: " << e.what() << endl;
    }
}