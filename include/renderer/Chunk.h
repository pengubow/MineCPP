#pragma once

#include "GL_compat.h"
#include "phys/AABB.h"
#include "level/Level.h"
#include "renderer/Tesselator.h"
#include "level/tile/Tile.h"
#include "renderer/Textures.h"
#include "player/Player.h"
#include "renderer/Frustum.h"

class Chunk {
    weak_ptr<Level> level;
    int32_t lists = -1;
    static shared_ptr<Tesselator> t;
public:
    static int32_t updates;
private:
    int32_t minX;
    int32_t minY;
    int32_t minZ;
    int32_t maxX;
    int32_t maxY;
    int32_t maxZ;
    vector<bool> skipRenderPass = vector<bool>(2);
public:
    bool isInFrustum = false;

    Chunk(shared_ptr<Level>& level, int32_t minX, int32_t minY, int32_t minZ, int32_t var5, int32_t var6);

    float distanceToSqr(shared_ptr<Player>& player);
private:
    void reset();
public:
    void clear();
    void rebuild();
    void render(vector<int32_t>& buffer, int32_t layer);
    void isInFrustumFunc(Frustum& frustum);
};