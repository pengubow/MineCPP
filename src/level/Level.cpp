#include <cstdint>
#include <vector>
#include <algorithm>
#include <zlib.h>
#include <iostream>
#include "level/Level.h"
#include "level/tile/Tile.h"
#include "level/LevelGen.h"
#include "Util.h"

using namespace std;

Level::Level(int32_t width, int32_t height, int32_t depth) 
    : width(width), 
    height(height), depth(depth), blocks(width * height * depth), 
    lightDepths(width * height) {
    bool mapLoaded = this->load();
    if (!mapLoaded) {
        unique_ptr<LevelGen> levelgen = make_unique<LevelGen>(width, height, depth);
        this->blocks = levelgen->generateMap();
    }
    this->calcLightDepths(0, 0, width, height);
}

bool Level::load() {
    try {
        gzFile dis = gzopen("level.dat", "rb");
        if (!dis) {
            throw runtime_error("level.dat not found.");
        }
        gzread(dis, this->blocks.data(), this->blocks.size());
        this->calcLightDepths(0, 0, this->width, this->height);
        for (int32_t i = 0; i < this->levelListeners.size(); i++) {
            this->levelListeners[i]->allChanged();
        }
        gzclose(dis);
        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading level.dat: " << e.what() << endl;
        return false;
    }
}

void Level::save() {
    try {
        gzFile dos = gzopen("level.dat", "wb");
        if (!dos) {
            throw runtime_error("Failed to open level.dat for writing.");
        }
        gzwrite(dos, this->blocks.data(), this->blocks.size());
        gzclose(dos);
    }
    catch (const exception& e) {
        cerr << "Error saving level.dat: " << e.what() << endl;
    }
}

void Level::calcLightDepths(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) {
    
    for (int32_t x = minX; x < minX + maxX; x++) {
        for (int32_t z = minY; z < minY + maxY; z++) {
            int32_t oldDepth = this->lightDepths[x + z * this->width];
            int32_t y = this->depth - 1;
            while (y > 0 && !this->isLightBlocker(x, y, z)) {
                --y;
            }
            this->lightDepths[x + z * this->width] = y;
            if (oldDepth != y) {
                int32_t yl0 = oldDepth < y ? oldDepth : y;
                int32_t yl1 = oldDepth > y ? oldDepth : y;
                for (int32_t i = 0; i < this->levelListeners.size(); i++) {
                    this->levelListeners[i]->lightColumnChanged(x, z, yl0, yl1);
                }
            }
        }
    }
}

void Level::addListener(LevelListener* levelListener) {
    this->levelListeners.push_back(levelListener);
}

void Level::removeListener(LevelListener* levelListener) {
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

vector<shared_ptr<AABB>> Level::getCubes(AABB aABB) {
    vector<shared_ptr<AABB>> aABBs = vector<shared_ptr<AABB>>();
    int32_t minX = aABB.minX;
    int32_t maxX = aABB.maxX + 1.0f;
    int32_t minY = aABB.minY;
    int32_t maxY = aABB.maxY + 1.0f;
    int32_t minZ = aABB.minZ;
    int32_t maxZ = aABB.maxZ + 1.0f;
    if (minX < 0) {
        minX = 0;
    }
    if (minY < 0) {
        minY = 0;
    }
    if (minZ < 0) {
        minZ = 0;
    }
    if (maxX > this->width) {
        maxX = this->width;
    }
    if (maxY > this->depth) {
        maxY = this->depth;
    }
    if (maxZ > this->height) {
        maxZ = this->height;
    }
    
    for (int32_t x = minX; x < maxX; x++) {
        for (int32_t y = minY; y < maxY; y++) {
            for (int32_t z = minZ; z < maxZ; z++) {
                shared_ptr<AABB> aabb;
                Tile* tile = Tile::tiles[this->getTile(x, y, z)];
                if (tile != nullptr && (aabb = tile->getAABB(x, y, z)) != nullptr) {
                    aABBs.push_back(aabb);
                }
            }
        }
    }
    return aABBs;
}

bool Level::setTile(int32_t x, int32_t y, int32_t z, int32_t type) {
    if (x < 0 || y < 0 || z < 0 || x >= this->width || y >= this->depth || z >= this->height) {
        return false;
    }
    if (type == this->blocks[(y * this->height + z) * this->width + x]) {
        return false;
    }
    this->blocks[(y * this->height + z) * this->width + x] = (uint8_t)type;
    this->calcLightDepths(x, z, 1, 1);
    
    for (int32_t i = 0; i < this->levelListeners.size(); i++) {
        this->levelListeners.at(i)->tileChanged(x, y, z);
    }
    return true;
}

bool Level::isLit(int32_t x, int32_t y, int32_t z) {
    if (x < 0 || y < 0 || z < 0 || x >= this->width || y >= this->depth || z >= this->height) {
        return true;
    }
    return y >= this->lightDepths[x + z * this->width];
}

int32_t Level::getTile(int32_t x, int32_t y, int32_t z) {
    if (x < 0 || y < 0 || z < 0 || x >= this->width || y >= this->depth || z >= this->height) {
        return 0;
    }
    return this->blocks[(y * this->height + z) * this->width + x];
}

bool Level::isSolidTile(int32_t x, int32_t y, int32_t z) {
    Tile* tile = Tile::tiles[this->getTile(x, y, z)];
    if (tile == nullptr) {
        return false;
    }
    return tile->isSolid();
}

void Level::tick() {
    this->unprocessed += this->width * this->height * this->depth;
    int ticks = this->unprocessed / 400;
    this->unprocessed -= ticks * 400;
    for (int32_t i = 0; i < ticks; i++) {
        int z;
        int y;
        int x = Util::nextInt(this->width);
        Tile* tile = Tile::tiles[this->getTile(x, y = Util::nextInt(this->depth), z = Util::nextInt(this->height))];
        if (tile != nullptr) {
            shared_ptr<Level> shared = shared_from_this();
            tile->tick(shared, x, y, z);
        }
    }
}