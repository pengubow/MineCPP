#include <GL/gl.h>
#include <iostream>
#include "Player.h"
#include "level/Chunk.h"
#include "Timer.h"
#include "Util.h"

std::shared_ptr<Tesselator> Chunk::t = Tesselator::instance == nullptr ? Tesselator::instance = std::make_shared<Tesselator>() : Tesselator::instance;
int32_t Chunk::updates = 0;
int64_t Chunk::totalTime = 0;
int32_t Chunk::totalUpdates = 0;

Chunk::Chunk(std::shared_ptr<Level>& level, int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ) 
    : aabb(minX, minY, minZ, maxX, maxY, maxZ), level(level),
    minX(minX), minY(minY), minZ(minZ),
    maxX(maxX), maxY(maxY), maxZ(maxZ), 
    x((float)(minX + maxX) / 2.0f), y((float)(minY + maxY) / 2.0f),
    z((float)(minZ + maxZ) / 2.0f) {
    this->lists = glGenLists(2);
}

void Chunk::rebuild(int32_t layer) {
    this->dirty = false;
    ++updates;
    int64_t before = Timer::nanoTime();
    glNewList(this->lists + layer, GL_COMPILE);
    t->init();
    int32_t tiles = 0;

    
    for (int32_t x = this->minX; x < this->maxX; x++) {
        for (int32_t y = this->minY; y < this->maxY; y++) {
            for (int32_t z = this->minZ; z < this->maxZ; z++) {
                int32_t tileId = this->level->getTile(x, y, z);
                if (tileId > 0) {
                    Tile::tiles[tileId]->render(t, this->level, layer, x, y, z);
                    ++tiles;
                }
            }
        }
    }

    t->flush();
    glEndList();
    int64_t after = Timer::nanoTime();
    if (tiles > 0) {
        totalTime += after - before;
        ++totalUpdates;
    }
}

void Chunk::rebuild() {
    this->rebuild(0);
    this->rebuild(1);
}

void Chunk::render(int32_t layer) {
    glCallList(this->lists + layer);
}

void Chunk::setDirty() {
    if (!this->dirty) {
        this->dirtiedTime = Timer::nanoTime() / 1000000;
    }
    this->dirty = true;
}

bool Chunk::isDirty() {
    return this->dirty;
}

float Chunk::distanceToSqr(std::shared_ptr<Player>& player) {
    float xd = player->x - this->x;
    float yd = player->y - this->y;
    float zd = player->z - this->z;
    return xd * xd + yd * yd + zd * zd;
}