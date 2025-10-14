#pragma once

#include <memory>
#include <iostream>
#include <zlib.h>
#include "level/Level.h"

class Minecraft;

class LevelIO {
private:
    shared_ptr<Minecraft> minecraft;
public:
    LevelIO(shared_ptr<Minecraft>& minecraft);
    shared_ptr<Level> load(gzFile file);
    shared_ptr<Level> loadDat(gzFile file);
    shared_ptr<Level> loadLegacy(gzFile file);
    static void save(shared_ptr<Level>& level, gzFile file);
};