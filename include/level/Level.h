#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "level/LevelListener.h"
#include "phys/AABB.h"

using namespace std;

class Level : public enable_shared_from_this<Level> {
private:
    static const int32_t TILE_UPDATE_INTERVAL = 400;
public:
    const int32_t width;
    const int32_t height;
    const int32_t depth;
private:
    vector<uint8_t> blocks;
    vector<int32_t> lightDepths;
    vector<LevelListener*> levelListeners = vector<LevelListener*>();
    int32_t unprocessed = 0;
public:
    Level(int32_t width, int32_t height, int32_t depth);

    void generateMap();
    bool load();
    void save();
    void calcLightDepths(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);
    void addListener(LevelListener* levelListener);
    void removeListener(LevelListener* levelListener);
    bool isLightBlocker(int32_t x, int32_t y, int32_t z);
    vector<shared_ptr<AABB>> getCubes(AABB aABB);
    bool setTile(int32_t x, int32_t y, int32_t z, int32_t type);
    bool isLit(int32_t x, int32_t y, int32_t z);
    int32_t getTile(int32_t x, int32_t y, int32_t z);
    bool isSolidTile(int32_t x, int32_t y, int32_t z);
    void tick();
};
