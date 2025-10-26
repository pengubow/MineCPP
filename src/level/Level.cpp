#include <cstdint>
#include <vector>
#include <array>
#include <algorithm>
#include <zlib.h>
#include <iostream>
#include <cmath>
#include "level/Level.h"
#include "level/tile/Tile.h"
#include "renderer/LevelRenderer.h"
#include "Util/Util.h"

using namespace std;

void Level::initTransient() {
    if (blocks.empty()) {
        throw runtime_error("The level is corrupt!");
    }
    else {
        levelListeners = vector<LevelRenderer*>();
        heightMap = vector<int32_t>(width * height, depth);
        this->calcLightDepths(0, 0, width, height);
        tickList = vector<shared_ptr<Coord>>();
    }

    if (xSpawn == 0 && ySpawn == 0 && zSpawn == 0) {
        findSpawn();
    }
}

void Level::setData(int32_t width, int32_t depth, int32_t height, vector<uint8_t> blocks) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->blocks = blocks;
    heightMap = vector<int32_t>(width * height, depth);
    calcLightDepths(0, 0, width, height);
    random = Random();
    randValue = random.nextInt();

    for (int32_t i = 0; i < levelListeners.size(); i++) {
        levelListeners[i]->compileSurroundingGround();
    }

    tickList.clear();
    findSpawn();
}

void Level::findSpawn() {
    Random random = Random();
    int32_t var2 = 0;

    int32_t x = 0;
    int32_t z = 0;
    int32_t y = 0;
    while ((float)y <= getWaterLevel()) {
        var2++;
        x = random.nextInt(width / 2) + width / 4;
        z = random.nextInt(height / 2) + height / 4;
        y = getHighestTile(x, z) + 1;
        if (var2 == 10000) {
            xSpawn = x;
            ySpawn = -100;
            zSpawn = z;
            return;
        }
    }

    xSpawn = x;
    ySpawn = y;
    zSpawn = z;
}

void Level::calcLightDepths(int32_t minX, int32_t minZ, int32_t maxX, int32_t maxZ) {
    for (int32_t x = minX; x < minX + maxX; x++) {
        for (int32_t z = minZ; z < minZ + maxZ; z++) {
            int32_t oldDepth = this->heightMap[x + z * this->width];

            int32_t y;
            for (y = this->depth - 1; y > 0 && !this->isLightBlocker(x, y, z); y--) {}

            this->heightMap[x + z * this->width] = y + 1;
            if (oldDepth != y) {
                int32_t yl0 = oldDepth < y ? oldDepth : y;
                int32_t yl1 = oldDepth > y ? oldDepth : y;

                for (int32_t i = 0; i < this->levelListeners.size(); i++) {
                    LevelRenderer* levelrenderer = levelListeners[i];
                    levelrenderer->setDirty(x - 1, yl0 - 1, z - 1, x + 1, oldDepth + 1, z + 1);
                }
            }
        }
    }
}

void Level::addListener(LevelRenderer* levelListener) {
    this->levelListeners.push_back(levelListener);
}

void Level::finalize() {}

void Level::removeListener(LevelRenderer* levelListener) {
    this->levelListeners.erase(
        remove(this->levelListeners.begin(), this->levelListeners.end(), levelListener),
        this->levelListeners.end()
    );
}

bool Level::isLightBlocker(int32_t x, int32_t y, int32_t z) {
    Tile* tile = Tile::tiles[this->getTile(x, y, z)];
    if (tile == nullptr) {
        return false;
    }
    return tile->blocksLight();
}

vector<AABB> Level::getCubes(AABB& aABB) {
    vector<AABB> aABBs = vector<AABB>();
    int32_t minX = aABB.minX;
    int32_t maxX = aABB.maxX + 1;
    int32_t minY = aABB.minY;
    int32_t maxY = aABB.maxY + 1;
    int32_t minZ = aABB.minZ;
    int32_t maxZ = aABB.maxZ + 1;
    if (aABB.minX < 0) {
        minX--;
    }
    if (aABB.minY < 0) {
        minY--;
    }
    if (aABB.minZ < 0) {
        minZ--;
    }
    
    for (int32_t x = minX; x < maxX; x++) {
        for (int32_t y = minY; y < maxY; y++) {
            for (int32_t z = minZ; z < maxZ; z++) {
                optional<AABB> aabb;
                if(x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
                    Tile* tile = Tile::tiles[this->getTile(x, y, z)];
                    if(tile != nullptr) {
                        aabb = tile->getAABB(x, y, z);
                        if(aabb.has_value()) {
                            aABBs.push_back(aabb.value());
                        }
                    }
                } else if(x < 0 || y < 0 || z < 0 || x >= width || z >= height) {
                    aabb = Tile::unbreakable->getAABB(x, y, z);
                    if(aabb.has_value()) {
                        aABBs.push_back(aabb.value());
                    }
                }
            }
        }
    }

    return aABBs;
}

void Level::swap(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ) {
    if (!networkMode) {
        int32_t var7 = getTile(minX, minY, minZ);
        int32_t var8 = getTile(maxX, maxY, maxZ);
        setTileNoNeighborChange(minX, minY, minZ, var8);
        setTileNoNeighborChange(maxX, maxY, maxZ, var7);
        updateNeighborAt(minX, minY, minZ, var8);
        updateNeighborAt(maxX, maxY, maxZ, var7);
    }
}

bool Level::setTileNoNeighborChange(int32_t x, int32_t y, int32_t z, int32_t type) {
    return networkMode ? false : netSetTileNoNeighborChange(x, y, z, type);
}

bool Level::netSetTileNoNeighborChange(int32_t x, int32_t y, int32_t z, int32_t type) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        if (type == blocks[(y * height + z) * width + x]) {
            return false;
        }
        else {
            if (type == 0 && (x == 0 || z == 0 || x == width - 1 || z == height - 1) && (float)y >= getGroundLevel() && (float)y < getWaterLevel()) {
                type = Tile::water->id;
            }

            uint8_t block = blocks[(y * height + z) * width + x];
            blocks[(y * height + z) * width + x] = (uint8_t)type;
            if (block != 0) {
                shared_ptr<Level> level = shared_from_this();
                Tile::tiles[block]->onTileRemoved(level, x, y, z);
            }

            if (type != 0) {
                shared_ptr<Level> level = shared_from_this();
                Tile::tiles[type]->onTileAdded(level, x, y, z);
            }

            calcLightDepths(x, z, 1, 1);

            for(int32_t var4 = 0; var4 < levelListeners.size(); ++var4) {
                LevelRenderer* var5 = levelListeners[var4];
                var5->setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
            }

            return true;
        }
    } else {
        return false;
    }
}

bool Level::setTile(int32_t x, int32_t y, int32_t z, int32_t type) {
    if (networkMode) {
        return false;
    }
    else if (setTileNoNeighborChange(x, y, z, type)) {
        updateNeighborsAt(x, y, z, type);
        return true;
    }
    else {
        return false;
    }
}

bool Level::netSetTile(int32_t x, int32_t y, int32_t z, int32_t type) {
    if (netSetTileNoNeighborChange(x, y, z, type)) {
        updateNeighborsAt(x, y, z, type);
        return true;
    }
    else {
        return false;
    }
}

void Level::updateNeighborsAt(int32_t x, int32_t y, int32_t z, int32_t type) {
    updateNeighborAt(x - 1, y, z, type);
    updateNeighborAt(x + 1, y, z, type);
    updateNeighborAt(x, y - 1, z, type);
    updateNeighborAt(x, y + 1, z, type);
    updateNeighborAt(x, y, z - 1, type);
    updateNeighborAt(x, y, z + 1, type);
}

bool Level::setTileNoUpdate(int32_t x, int32_t y, int32_t z, int32_t type) {
    if(x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        if(type == blocks[(y * height + z) * width + x]) {
            return false;
        } else {
            blocks[(y * height + z) * width + x] = (uint8_t)type;
            return true;
        }
    } else {
        return false;
    }
}

void Level::updateNeighborAt(int32_t x, int32_t y, int32_t z, int32_t type) {
    if (x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height) {
        Tile* tile = Tile::tiles[blocks[(y * height + z) * width + x]];
        if (tile != nullptr) {
            shared_ptr<Level> shared = shared_from_this();
            tile->neighborChanged(shared, x, y, z, type);
        }
    }
}

bool Level::isLit(int32_t x, int32_t y, int32_t z) {
    return x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height ? y >= heightMap[x + z * width] : true;
}

int32_t Level::getTile(int32_t x, int32_t y, int32_t z) {
    return x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height ? blocks[(y * height + z) * width + x] & 255 : 0;
}

bool Level::isSolidTile(int32_t x, int32_t y, int32_t z) {
    Tile* tile = Tile::tiles[getTile(x, y, z)];
	return tile == nullptr ? false : tile->isSolid();
}

void Level::tickEntities() {
    for (int32_t var1 = 0; var1 < entities.size(); ++var1) {
        entities[var1]->tick();
        if (entities[var1]->removed) {
            entities.erase(entities.begin() + var1--);
        }
    }
}

void Level::tick() {
    shared_ptr<Level> shared = shared_from_this();
    tickCount++;
    int32_t var1 = 1;

    int32_t var2;
    for (var2 = 1; 1 << var1 < width; ++var1) {}

    while (1 << var2 < height) {
        ++var2;
    }

    int32_t var3 = height - 1;
    int32_t var4 = width - 1;
    int32_t var5 = depth - 1;
    int32_t var6;
    int32_t var7;
    if (tickCount % 5 == 0) {
        var6 = tickList.size();

        for (var7 = 0; var7 < var6; ++var7) {
            shared_ptr<Coord> coord = tickList[0];
            tickList.erase(tickList.begin());
            if (coord->scheduledTime > 0) {
                coord->scheduledTime--;
                tickList.push_back(coord); 
            }
            else if (isInLevelBounds(coord->x, coord->y, coord->z)) {
                uint8_t var9 = blocks[(coord->y * height + coord->z) * width + coord->x];
                if (var9 == coord->id && var9 > 0) {
                    Tile::tiles[var9]->tick(shared, coord->x, coord->y, coord->z, random);
                }
            }
        }
    }

    unprocessed += width * height * depth;
    var6 = unprocessed / 200;
    unprocessed -= var6 * 200;

    for(var7 = 0; var7 < var6; ++var7) {
        randValue = randValue * 3 + 1013904223;
        int32_t var12 = randValue >> 2;
        int32_t var13 = var12 & var4;
        int32_t var10 = var12 >> var1 & var3;
        var12 = var12 >> var1 + var2 & var5;
        uint8_t var11 = blocks[(var12 * height + var10) * width + var13];
        if(Tile::shouldTick[var11]) {
            Tile::tiles[var11]->tick(shared, var13, var12, var10, random);
        }
    }
}

bool Level::isInLevelBounds(int32_t x, int32_t y, int32_t z) {
    return x >= 0 && y >= 0 && z >= 0 && x < width && y < depth && z < height;
}

float Level::getGroundLevel() {
    return (float)(depth / 2 - 2);
}

float Level::getWaterLevel() {
    return (float)(depth / 2);
}

bool Level::containsAnyLiquid(AABB& aabb) {
    int32_t minX = (int32_t)aabb.minX;
    int32_t maxX = (int32_t)aabb.maxX + 1;
    int32_t minY = (int32_t)aabb.minY;
    int32_t maxY = (int32_t)aabb.maxY + 1;
    int32_t minZ = (int32_t)aabb.minZ;
    int32_t maxZ = (int32_t)aabb.maxZ + 1;
    if (aabb.minX < 0.0F) {
        minX--;
    }

    if (aabb.minY < 0.0F) {
        minY--;
    }

    if (aabb.minZ < 0.0F) {
        minZ--;
    }

    if (minX < 0) {
        minX = 0;
    }

    if (minY < 0) {
        minY = 0;
    }

    if (minZ < 0) {
        minZ = 0;
    }

    if (maxX > width) {
        maxX = width;
    }

    if (maxY > depth) {
        maxY = depth;
    }

    if (maxZ > height) {
        maxZ = height;
    }

    for (int32_t x = minX; x < maxX; x++) {
        for (int32_t y = minY; y < maxY; y++) {
            for (int32_t z = minZ; z < maxZ; z++) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() != Liquid::none) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Level::containsLiquid(AABB& aabb, Liquid* liquidId) {
    int32_t minX = aabb.minX;
    int32_t maxX = aabb.maxX + 1;
    int32_t minY = aabb.minY;
    int32_t maxY = aabb.maxY + 1;
    int32_t minZ = aabb.minZ;
    int32_t maxZ = aabb.maxZ + 1;

    if (aabb.minX < 0.0f) {
        minX--;
    }

    if (aabb.minY < 0.0f) {
        minY--;
    }

    if (aabb.minZ < 0.0f) {
        minZ--;
    }

    if (minX < 0) {
        minX = 0;
    }

    if (minY < 0) {
        minY = 0;
    }

    if (minZ < 0) {
        minZ = 0;
    }

    if (maxX > width) {
        maxX = width;
    }

    if (maxY > depth) {
        maxY = depth;
    }

    if (maxZ > height) {
        maxZ = height;
    }

    for (int32_t x = minX; x < maxX; x++) {
        for (int32_t y = minY; y < maxY; y++) {
            for (int32_t z = minZ; z < maxZ; z++) {
                Tile* tile = Tile::tiles[getTile(x, y, z)];
                if (tile != nullptr && tile->getLiquidType() == liquidId) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Level::addToTickNextTick(int32_t x, int32_t y, int32_t z, int32_t id) {
    if (!networkMode) {
        shared_ptr<Coord> coord = make_shared<Coord>(x, y, z, id);
        if(id > 0) {
            int32_t tickDelay = Tile::tiles[id]->getTickDelay();
            coord->scheduledTime = tickDelay;
        }

        tickList.push_back(coord);
    }
}

bool Level::isFree(AABB& aabb) {
    for (int32_t i = 0; i < entities.size(); i++) {
        if (entities[i]->bb.intersects(aabb)) {
            return false;
        }
    }

    return true;
}

bool Level::isSolid(float var1, float var2, float var3, float var4) {
    return isSolidTile(var1 - var4, var2 - var4, var3 - var4) ? true : (isSolidTile(var1 - var4, var2 - var4, var3 + var4) ? true : (isSolidTile(var1 - var4, var2 + var4, var3 - var4) ? true : (isSolidTile(var1 - var4, var2 + var4, var3 + var4) ? true : (isSolidTile(var1 + var4, var2 - var4, var3 - var4) ? true : (isSolidTile(var1 + var4, var2 - var4, var3 + var4) ? true : (isSolidTile(var1 + var4, var2 + var4, var3 - var4) ? true : isSolidTile(var1 + var4, var2 + var4, var3 + var4)))))));
}

bool Level::isSolidTile(float x, float y, float z) {
    int32_t tile = getTile((int32_t)x, (int32_t)y, (int32_t)z);
	return tile > 0 && Tile::tiles[tile]->isSolid();
}

int32_t Level::getHighestTile(int32_t x, int32_t z) {
    int32_t y;
    for (y = depth; (getTile(x, y - 1, z) == 0 || Tile::tiles[getTile(x, y - 1, z)]->getLiquidType() != Liquid::none) && y > 0; y--) {}

    return y;
}

void Level::setSpawnPos(int32_t x, int32_t y, int32_t z, float rot) {
    xSpawn = x;
    ySpawn = y;
    zSpawn = z;
    rotSpawn = rot;
}

float Level::getBrightness(int32_t x, int32_t y, int32_t z) {
    return isLit(x, y, z) ? 1.0f : 0.6f;
}

float Level::getCaveness(float x, float y, float z, float var4) {
    int32_t var5 = x;
    int32_t var14 = y;
    int32_t var6 = z;
    float var7 = 0.0f;
    float var8 = 0.0f;

    for (int32_t var9 = var5 - 6; var9 <= var5 + 6; var9++) {
        for (int32_t var10 = var6 - 6; var10 <= var6 + 6; var10++) {
            if (isInLevelBounds(var9, var14, var10) && !isSolidTile(var9, var14, var10)) {
                float var11 = (float)var9 + 0.5f - x;
                float var12 = (float)var10 + 0.5f - z;

                float var13;
                for (var13 = (float)(atan2((double)var12, (double)var11) - (double)var4 * M_PI / 180.0 + M_PI * 0.5); (double)var13 < -M_PI; var13 = (float)((double)var13 + M_PI * 2.0)) {}

                while ((double)var13 >= M_PI) {
                    var13 = (float)((double)var13 - M_PI * 2.0);
                }

                if (var13 < 0.0f) {
                    var13 = -var13;
                }

                var11 = (float)sqrt((double)(var11 * var11 + 4.0f + var12 * var12));
                var11 = 1.0f / var11;
                if (var13 > 1.0f) {
                    var11 = 0.0f;
                }

                if (var11 < 0.0f) {
                    var11 = 0.0f;
                }

                var8 += var11;
                if (isLit(var9, var14, var10)) {
                    var7 += var11;
                }
            }
        }
    }

    if (var8 == 0.0f) {
        return 0.0f;
    } 
    else {
        return var7 / var8;
    }
}

float Level::getCaveness(shared_ptr<Entity>& entity) {
    float var2 = (float)cos((double)(-entity->yRot) * M_PI / 180.0 + M_PI);
    float var3 = (float)sin((double)(-entity->yRot) * M_PI / 180.0 + M_PI);
    float var4 = (float)cos((double)(-entity->xRot) * M_PI / 180.0);
    float var5 = (float)sin((double)(-entity->xRot) * M_PI / 180.0);
    float var6 = entity->x;
    float var7 = entity->y;
    float var21 = entity->z;
    float var8 = 1.6f;
    float var9 = 0.0f;
    float var10 = 0.0f;

    for (int32_t var11 = 0; var11 <= 200; ++var11) {
        float var12 = ((float)var11 / (float)200 - 0.5f) * 2.0f;

        for (int32_t var13 = 0; var13 <= 200; ++var13) {
            float var14 = ((float)var13 / (float)200 - 0.5f) * var8;
            float var16 = var4 * var14 + var5;
            var14 = var4 - var5 * var14;
            float var17 = var2 * var12 + var3 * var14;
            var14 = var2 * var14 - var3 * var12;

            for (int var15 = 0; var15 < 10; ++var15) {
                float var18 = var6 + var17 * (float)var15 * 0.8f;
                float var19 = var7 + var16 * (float)var15 * 0.8f;
                float var20 = var21 + var14 * (float)var15 * 0.8f;
                if(isSolidTile(var18, var19, var20)) {
                    break;
                }

                var9++;
                if (isLit((int)var18, (int)var19, (int)var20)) {
                    var10++;
                }
            }
        }
    }

    if (var9 == 0.0f) {
        return 0.0f;
    } 
    else {
        float var22 = var10 / var9;
        var22 /= 0.1f;
        if(var22 > 1.0f) {
            var22 = 1.0f;
        }

        var22 = 1.0f - var22;
        return 1.0f - var22 * var22 * var22;
    }
}

vector<uint8_t> Level::copyBlocks() {
    return blocks;
}

bool Level::isWater(int32_t x, int32_t y, int32_t z) {
    int32_t tile = getTile(x, y, z);
    return tile > 0 && Tile::tiles[tile]->getLiquidType() == Liquid::water;
}

void Level::setNetworkMode(bool networkMode) {
    this->networkMode = networkMode;
}