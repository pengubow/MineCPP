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
    fs::path resourcesFolder;
    weak_ptr<Minecraft> minecraft;
public:
    atomic<bool> closing = false;

    BackgroundDownloader(const fs::path& workDir, shared_ptr<Minecraft>& minecraft);
    void stop();
private:
    void downloadResource(const string& url, const fs::path& file);
    void run();
};