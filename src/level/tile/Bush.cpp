#include <cmath>
#include "level/tile/Bush.h"
#include <iostream>
Bush::Bush(int32_t id) : Tile(id) {
    this->tex = 15;
}

void Bush::tick(std::shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    int32_t below = level->getTile(x, y - 1, z);
    const char* hi = level->isLit(x, y, z) == true ? "yes" : "no";
    std::cout << hi << std::endl;
    if (!level->isLit(x, y, z) || below != Tile::dirt->id && below != Tile::grass->id) {
        level->setTile(x, y, z, 0);
    }
}

void Bush::render(std::shared_ptr<Tesselator>& t, std::shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) {
    if (level->isLit(x, y, z) ^ layer != 1) {
        return;
    }
    int32_t tex = this->getTexture(15);
    float u0 = (float)(tex % 16) / 16.0f;
    float u1 = u0 + 0.0624375f;
    float v0 = (float)(tex / 16) / 16.0f;
    float v1 = v0 + 0.0624375f;
    int32_t rots = 2;
    t->color(1.0f, 1.0f, 1.0f);
    for (int32_t r = 0; r < rots; r++) {
        float xa = (float)(std::sin((double)r * M_PI / (double)rots + 0.7853981633974483) * 0.5);
        float za = (float)(std::cos((double)r * M_PI / (double)rots + 0.7853981633974483) * 0.5);
        float x0 = (float)x + 0.5f - xa;
        float x1 = (float)x + 0.5f + xa;
        float y0 = (float)y + 0.0f;
        float y1 = (float)y + 1.0f;
        float z0 = (float)z + 0.5f - za;
        float z1 = (float)z + 0.5f + za;
        t->vertexUV(x0, y1, z0, u1, v0);
        t->vertexUV(x1, y1, z1, u0, v0);
        t->vertexUV(x1, y0, z1, u0, v1);
        t->vertexUV(x0, y0, z0, u1, v1);
        t->vertexUV(x1, y1, z1, u0, v0);
        t->vertexUV(x0, y1, z0, u1, v0);
        t->vertexUV(x0, y0, z0, u1, v1);
        t->vertexUV(x1, y0, z1, u0, v1);
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