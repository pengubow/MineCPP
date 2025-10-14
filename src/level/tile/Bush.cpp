#include <cmath>
#include "level/tile/Bush.h"

Bush::Bush(int32_t id) 
    : Tile(id) {
    this->tex = 15;
    setTicking(true);
}

void Bush::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    int32_t below = level->getTile(x, y - 1, z);
    if (!level->isLit(x, y, z) || below != Tile::dirt->id && below != Tile::grass->id) {
        level->setTile(x, y, z, 0);
    }
}

bool Bush::render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) {
    if (level->isLit(x, y, z) ^ layer != 1) {
        return false;
    }
    else {
        int32_t tex = this->getTexture(15);
        float minU = (float)(tex % 16) / 16.0f;
        float maxU = minU + 0.999f / 16.0f;
        float minV = (float)(tex / 16) / 16.0f;
        float maxV = minV + 0.999f / 16.0f;
        int32_t rots = 2;
        t->color(1.0f, 1.0f, 1.0f);
        for (int32_t r = 0; r < rots; r++) {
            float xa = (float)(sin((double)r * M_PI / (double)rots + M_PI * 0.25) * 0.5);
            float za = (float)(cos((double)r * M_PI / (double)rots + M_PI * 0.25) * 0.5);
            float minX = (float)x + 0.5f - xa;
            float maxX = (float)x + 0.5f + xa;
            float minY = (float)y;
            float maxY = (float)y + 1.0f;
            float minZ = (float)z + 0.5f - za;
            float maxZ = (float)z + 0.5f + za;
            t->vertexUV(minX, maxY, minZ, maxU, minV);
            t->vertexUV(maxX, maxY, maxZ, minU, minV);
            t->vertexUV(maxX, minY, maxZ, minU, maxV);
            t->vertexUV(minX, minY, minZ, maxU, maxV);
            t->vertexUV(maxX, maxY, maxZ, minU, minV);
            t->vertexUV(minX, maxY, minZ, maxU, minV);
            t->vertexUV(minX, minY, minZ, maxU, maxV);
            t->vertexUV(maxX, minY, maxZ, minU, maxV);
        }
        return true;
    }
}

shared_ptr<AABB> Bush::getAABB(int32_t x, int32_t y, int32_t z) {
    return nullptr;
}

bool Bush::blocksLight() {
    return false;
}

bool Bush::isSolid() {
    return false;
}