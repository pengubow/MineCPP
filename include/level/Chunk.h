#pragma once

#include <GL/gl.h>
#include "phys/AABB.h"
#include "level/Level.h"
#include "level/Tesselator.h"
#include "level/tile/Tile.h"
#include "Textures.h"
#include "Player.h"

class Chunk {
public:
    AABB aabb;
    std::shared_ptr<Level> level;
    const int32_t minX;
    const int32_t minY;
    const int32_t minZ;
    const int32_t maxX;
    const int32_t maxY;
    const int32_t maxZ;
    const float x;
    const float y;
    const float z;
private:
    bool dirty = true;
    int32_t lists = -1;
public:
    int64_t dirtiedTime = 0;
private:
    static std::shared_ptr<Tesselator> t;
public:
    static int32_t updates;
private:
    static int64_t totalTime;
    static int32_t totalUpdates;
public:
    Chunk(std::shared_ptr<Level>& level, int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);

private:
    void rebuild(int32_t layer);
public:
    void rebuild();
    void render(int32_t layer);
    void setDirty();
    bool isDirty();
    float distanceToSqr(std::shared_ptr<Player>& player);
};