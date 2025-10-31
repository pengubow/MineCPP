#include <iostream>
#include <fstream>
#include <sstream>
#include "Util/httplib.h"
#include "BackgroundDownloader.h"
#include "Minecraft.h"
#include "sound/SoundManager.h"

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
#else
    #include <unistd.h>
    #include <pwd.h>
#endif

BackgroundDownloader::OS BackgroundDownloader::detectOS() {
    #ifdef _WIN32
        return OS::WINDOWS_OS;
    #elif __APPLE__
        return OS::MACOS;
    #elif __linux__
        return OS::LINUX;
    #elif __sun
        return OS::SOLARIS;
    #else
        return OS::UNKNOWN;
    #endif
}

fs::path BackgroundDownloader::getResourcesFolder() {
    string homeDir;
    
    #ifdef _WIN32
        char* appdata = getenv("APPDATA");
        if (appdata) {
            homeDir = appdata;
        } else {
            char* userProfile = getenv("USERPROFILE");
            homeDir = userProfile ? userProfile : ".";
        }
    #else
        const char* home = getenv("HOME");
        if (home) {
            homeDir = home;
        } else {
            struct passwd* pw = getpwuid(getuid());
            homeDir = pw ? pw->pw_dir : ".";
        }
    #endif

    OS os = detectOS();
    fs::path baseDir;

    switch (os) {
        case OS::WINDOWS_OS:
            baseDir = fs::path(homeDir) / ".minecraft";
            break;
        case OS::MACOS:
            baseDir = fs::path(homeDir) / "Library/Application Support/minecraft";
            break;
        case OS::LINUX:
        case OS::SOLARIS:
        default:
            baseDir = fs::path(homeDir) / ".minecraft";
            break;
    }

    fs::path resourcesDir = baseDir / "resources";
    if (!fs::exists(resourcesDir)) {
        fs::create_directories(resourcesDir);
    }

    return resourcesDir;
}

BackgroundDownloader::BackgroundDownloader(shared_ptr<Minecraft> minecraft)
    : minecraft(minecraft) {
    
    resourcesFolder = getResourcesFolder();
    if (!fs::exists(resourcesFolder)) {
        throw runtime_error("Failed to create resources folder: " + resourcesFolder.string());
    }

    thread t(&BackgroundDownloader::run, this);
    t.detach();
}

void BackgroundDownloader::stop() {
    closing = true;
}

void BackgroundDownloader::downloadResource(const string& url, const fs::path& file) {
    size_t schemeEnd = url.find("://");
    if (schemeEnd == string::npos) {
        return;
    }
    
    size_t hostStart = schemeEnd + 3;
    size_t pathStart = url.find('/', hostStart);
    
    string host;
    string path;
    
    if (pathStart == string::npos) {
        host = url.substr(hostStart);
        path = "/";
    } else {
        host = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }
    
    httplib::Client cli(host);
    cli.set_connection_timeout(0, 500000);
    cli.set_read_timeout(5, 0);

    auto res = cli.Get(path);
    if (!res || res->status != 200) {
        cerr << "Failed to download: " << url << " (status: " << (res ? res->status : -1) << ")" << endl;
        return;
    }

    ofstream outfile(file, ios::binary);
    if (!outfile) {
        cerr << "Failed to open file: " << file << endl;
        return;
    }

    outfile.write(res->body.c_str(), res->body.size());
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