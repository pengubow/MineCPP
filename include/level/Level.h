#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "phys/AABB.h"
#include "level/Coord.h"
#include "Util/Util.h"

using namespace std;
class LevelRenderer;
class Entity;

class Level : public enable_shared_from_this<Level> {
public:
    static const int64_t serialVersionUID = 0;
    int32_t width;
    int32_t height;
    int32_t depth;
// private:
    vector<uint8_t> blocks;
// public:
    string name;
    string creator;
    int64_t createTime;
    int32_t xSpawn;
    int32_t ySpawn;
    int32_t zSpawn;
    float rotSpawn;
private:
    vector<shared_ptr<LevelRenderer>> levelListeners;
    vector<int32_t> heightMap;
    int32_t randValue = Util::nextInt();
    vector<shared_ptr<Coord>> tickList;
public:
    vector<shared_ptr<Entity>> entities;
    int32_t unprocessed = 0;
    int32_t tickCount = 0;

    void initTransient();
    void setData(int32_t width, int32_t depth, int32_t height, vector<uint8_t> blocks);
    void findSpawn();
    void calcLightDepths(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);
    void addListener(shared_ptr<LevelRenderer>& levelListener);
    void finalize();
    void removeListener(shared_ptr<LevelRenderer>& levelListener);
    bool isLightBlocker(int32_t x, int32_t y, int32_t z);
    vector<shared_ptr<AABB>> getCubes(shared_ptr<AABB>& aABB);
    void swap(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);
    bool setTileNoNeighborChange(int32_t x, int32_t y, int32_t z, int32_t type);
    bool setTile(int32_t x, int32_t y, int32_t z, int32_t type);
private:
    void updateNeighborAt(int32_t x, int32_t y, int32_t z, int32_t type);
public:
    bool setTileNoUpdate(int32_t x, int32_t y, int32_t z, int32_t type);
private:
    void neighborChanged(int32_t x, int32_t y, int32_t z, int32_t type);
public:
    bool isLit(int32_t x, int32_t y, int32_t z);
    int32_t getTile(int32_t x, int32_t y, int32_t z);
    void tick();
private:
    bool isInLevelBounds(int32_t x, int32_t y, int32_t z);
public:
    float getGroundLevel();
    float getWaterLevel();
    bool containsAnyLiquid(shared_ptr<AABB>& aabb);
    bool containsLiquid(shared_ptr<AABB>& aabb, int32_t liquidId);
    void addToTickNextTick(int32_t x, int32_t y, int32_t z, int32_t id);
    bool isFree(shared_ptr<AABB>& aabb);
    bool isSolid(float var1, float var2, float var3, float var4);
    bool isSolidTile(int32_t x, int32_t y, int32_t z);
public:
    int32_t getHighestTile(int32_t x, int32_t z);
    void setSpawnPos(int32_t x, int32_t y, int32_t z, float rot);
    float getBrightness(int32_t x, int32_t y, int32_t z);
    float getCaveness(float x, float y, float z, float var4);
    float getCaveness(shared_ptr<Entity>& entity);
};
