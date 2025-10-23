#pragma once

#include <memory>
#include <iostream>
#include <zlib.h>
#include "level/Level.h"

class Minecraft;

class LevelIO {
    weak_ptr<Minecraft> minecraft;
public:
    LevelIO(shared_ptr<Minecraft>& minecraft);

    shared_ptr<Level> load(const string& serverUri, const string& username, int levelId);
    shared_ptr<Level> load(gzFile file);
    shared_ptr<Level> loadDat(gzFile file);
    shared_ptr<Level> loadLegacy(gzFile file);
    bool save(shared_ptr<Level>& level, string minecraftUri, string username, string sessionId, string levelName, int32_t levelId);
    static void save(shared_ptr<Level>& level, gzFile file);
    static vector<uint8_t> loadBlocks(gzFile file);
    static vector<uint8_t> loadBlocks(vector<uint8_t>& in);
};