#include <iostream>
#include "GL_compat.h"
#include "player/Player.h"
#include "renderer/Chunk.h"
#include "Timer.h"
#include "Util/Util.h"

shared_ptr<Tesselator> Chunk::t = Tesselator::instance == nullptr ? Tesselator::instance = make_shared<Tesselator>() : Tesselator::instance;
int32_t Chunk::updates = 0;

Chunk::Chunk(shared_ptr<Level>& level, int32_t minX, int32_t minY, int32_t minZ, int32_t var5)
    : level(level), minX(minX), minY(minY), minZ(minZ),
    maxX(16), maxY(16), maxZ(16) {
    sqrt((double)(this->maxX * this->maxX + this->maxY * this->maxY + this->maxZ * this->maxZ));
    this->lists = glGenLists(2);
    reset();
}

float Chunk::distanceToSqr(shared_ptr<Player>& player) {
    float xd = player->x - this->minX;
    float yd = player->y - this->minY;
    float zd = player->z - this->minZ;
    return xd * xd + yd * yd + zd * zd;
}

void Chunk::reset() {
    for(int32_t i = 0; i < 2; ++i) {
        glNewList(lists + i, GL_COMPILE);
        glEndList();
    }
}

void Chunk::clear() {
    glDeleteLists(lists, 2);
    level;
}

void Chunk::rebuild() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }
    ++updates;
    for (int32_t layer = 0; layer < 2; layer++) {
        int32_t x = this->minX;
        int32_t var5 = this->minY;
        int32_t var6 = this->minZ;
        int32_t var7 = this->minX + this->maxX;
        int32_t var8 = this->minY + this->maxY;
        int32_t var9 = this->minZ + this->maxZ;
        glNewList(lists + layer, GL_COMPILE);
        t->begin();
        bool var10 = false;

        for (x = x; x < var7; x++) {
            for (int32_t y = var5; y < var8; ++y) {
                for (int32_t z = var6; z < var9; z++) {
                    int32_t var13 = level->getTile(x, y, z);
                    if (var13 > 0) {
                        var10 |= Tile::tiles[var13]->render(t, level, layer, x, y, z);
                    }
                }
            }
        }

        t->end();
        glEndList();
        if (skipRenderPass[layer] != !var10) {
            skipRenderPass[layer] = !var10;
        }
    }
}

void Chunk::render(vector<int32_t>& buffer, int32_t layer) {
    if (isInFrustum && !skipRenderPass[layer]) {
        buffer.push_back(lists + layer);
    }
}

void Chunk::isInFrustumFunc(Frustum& frustum) {
    isInFrustum = frustum.cubeInFrustum((float)minX, (float)minY, (float)minZ, (float)(minX + maxX), (float)(minY + maxY), (float)(minZ + maxZ));
}