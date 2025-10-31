#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>
#include "character/Vec3.h"
#include "phys/AABB.h"
#include "level/Coord.h"
#include "level/liquid/Liquid.h"
#include "HitResult.h"
#include "Util/Util.h"
#include "Util/Random.h"

using namespace std;
class LevelRenderer;
class Entity;
class Minecraft;

class Level : public enable_shared_from_this<Level> {
public:
    static const int64_t serialVersionUID = 0;
    int32_t width;
    int32_t height;
    int32_t depth;
    vector<uint8_t> blocks;
    string name;
    string creator;
    int64_t createTime;
    int32_t xSpawn;
    int32_t ySpawn;
    int32_t zSpawn;
    float rotSpawn;
private:
    vector<LevelRenderer*> levelListeners;
    vector<int32_t> heightMap;
    Random random = Random();
    int32_t randValue = random.nextInt();
    vector<shared_ptr<Coord>> tickList;
public:
    vector<shared_ptr<Entity>> entities;
private:
    bool networkMode = false;
public:
    weak_ptr<Minecraft> rendererContext;
    int32_t unprocessed = 0;
    int32_t tickCount = 0;

    void initTransient();
    void setData(int32_t width, int32_t depth, int32_t height, vector<uint8_t> blocks);
    void findSpawn();
    void calcLightDepths(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);
    void addListener(LevelRenderer* levelListener);
    void finalize();
    void removeListener(LevelRenderer* levelListener);
    bool isLightBlocker(int32_t x, int32_t y, int32_t z);
    vector<AABB> getCubes(AABB& aABB);
    void swap(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);
    bool setTileNoNeighborChange(int32_t x, int32_t y, int32_t z, int32_t type);
    bool netSetTileNoNeighborChange(int32_t x, int32_t y, int32_t z, int32_t type);
    bool setTile(int32_t x, int32_t y, int32_t z, int32_t type);
    bool netSetTile(int32_t x, int32_t y, int32_t z, int32_t type);
    void updateNeighborsAt(int32_t x, int32_t y, int32_t z, int32_t type);
public:
    bool setTileNoUpdate(int32_t x, int32_t y, int32_t z, int32_t type);
private:
    void updateNeighborAt(int32_t x, int32_t y, int32_t z, int32_t type);
public:
    bool isLit(int32_t x, int32_t y, int32_t z);
    int32_t getTile(int32_t x, int32_t y, int32_t z);
    bool isSolidTile(int32_t x, int32_t y, int32_t z);
    void tickEntities();
    void tick();
private:
    bool isInLevelBounds(int32_t x, int32_t y, int32_t z);
public:
    float getGroundLevel();
    float getWaterLevel();
    bool containsAnyLiquid(AABB& aabb);
    bool containsLiquid(AABB& aabb, Liquid* liquidId);
    void addToTickNextTick(int32_t x, int32_t y, int32_t z, int32_t id);
    bool isFree(AABB& aabb);
    bool isSolid(float var1, float var2, float var3, float var4);
    bool isSolidTile(float x, float y, float z);
public:
    int32_t getHighestTile(int32_t x, int32_t z);
    void setSpawnPos(int32_t x, int32_t y, int32_t z, float rot);
    float getBrightness(int32_t x, int32_t y, int32_t z);
    float getCaveness(float x, float y, float z, float var4);
    float getCaveness(shared_ptr<Entity>& entity);
    vector<uint8_t> copyBlocks();
    bool isWater(int32_t x, int32_t y, int32_t z);
    void setNetworkMode(bool networkMode);
    optional<HitResult> clip(Vec3& var1, Vec3& var2);
    void playSound(string var1, shared_ptr<Entity> var2, float var3, float var4);
    void playSound(string var1, float var2, float var3, float var4, float var5, float var6);
};
