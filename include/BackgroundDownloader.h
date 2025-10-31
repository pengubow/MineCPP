#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class Minecraft;

class BackgroundDownloader {
private:
    enum class OS {
        LINUX,
        SOLARIS,
        WINDOWS_OS,
        MACOS,
        UNKNOWN
    };

    fs::path resourcesFolder;
    weak_ptr<Minecraft> minecraft;
public:
    atomic<bool> closing =false;

    static OS detectOS();
    static fs::path getResourcesFolder();
    void downloadResource(const string& url, const fs::path& file);
    void run();
public:
    BackgroundDownloader(shared_ptr<Minecraft> minecraft);
    
    void stop();
};