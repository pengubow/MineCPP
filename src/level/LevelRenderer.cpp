#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include "level/LevelRenderer.h"
#include "level/Chunk.h"
#include "level/tile/Tile.h"
#include "level/DirtyChunkSorter.h"
#include "Timer.h"

LevelRenderer::LevelRenderer(std::shared_ptr<Level>& level)
    : level(level) {
    level->addListener(this);

    xChunks = level->width / CHUNK_SIZE;
    yChunks = level->depth / CHUNK_SIZE;
    zChunks = level->height / CHUNK_SIZE;

    chunks.resize(xChunks * yChunks * zChunks, nullptr);

    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                int32_t minX = x * CHUNK_SIZE;
                int32_t minY = y * CHUNK_SIZE;
                int32_t minZ = z * CHUNK_SIZE;
                int32_t maxX = (x + 1) * CHUNK_SIZE;
                int32_t maxY = (y + 1) * CHUNK_SIZE;
                int32_t maxZ = (z + 1) * CHUNK_SIZE;
                if (maxX > level->width) maxX = level->width;
                if (maxY > level->depth) maxY = level->depth;
                if (maxZ > level->height) maxZ = level->height;

                chunks[(x + y * xChunks) * zChunks + z] =
                    new Chunk(level, minX, minY, minZ, maxX, maxY, maxZ);
            }
        }
    }
}

std::vector<Chunk*> LevelRenderer::getAllDirtyChunks() {
    std::vector<Chunk*> dirty;
    for (int32_t i = 0; i < this->chunks.size(); i++) {
        Chunk* chunk = this->chunks[i];
        if (chunk->isDirty()) {
            dirty.push_back(chunk);
        }
    }
    return dirty;
}

void LevelRenderer::render(std::shared_ptr<Player>& player, int32_t layer) {
    glEnable(GL_TEXTURE_2D);
    int32_t id = Textures::loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    Frustum& frustum = Frustum::getFrustum();
    for (int32_t i = 0; i < this->chunks.size(); i++) {
        if (frustum.isVisible(this->chunks.at(i)->aabb)) {
            this->chunks.at(i)->render(layer);
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void LevelRenderer::updateDirtyChunks(std::shared_ptr<Player> player) {
     std::vector<Chunk*> dirty = this->getAllDirtyChunks();
     if (dirty.empty()) {
        return;
     }
     DirtyChunkSorter sorter(player, Frustum::getFrustum());
     std::sort(dirty.begin(), dirty.end(), sorter);
     for (int32_t i = 0; i < 8 && i < dirty.size(); i++) {
        dirty.at(i)->rebuild();
     }
}

void LevelRenderer::pick(std::shared_ptr<Player>& player, Frustum& frustum) {
    std::shared_ptr<Tesselator> t = Tesselator::instance;
    float r = 3.0f;
    AABB box = player->bb.grow(r, r, r);

    int32_t minX = box.minX;
    int32_t maxX = box.maxX + 1.0f;
    int32_t minY = box.minY;
    int32_t maxY = box.maxY + 1.0f;
    int32_t minZ = box.minZ;
    int32_t maxZ = box.maxZ + 1.0f;

    glInitNames();
    glPushName(0);
    glPushName(0);

    for (int x = minX; x < maxX; ++x) {
        glLoadName(x);
        glPushName(0);
        for (int y = minY; y < maxY; ++y) {
            glLoadName(y);
            glPushName(0);
            for (int z = minZ; z < maxZ; ++z) {
                Tile* tile = Tile::tiles[this->level->getTile(x, y, z)];
                if (tile != nullptr && frustum.isVisible(*tile->getTileAABB(x, y, z))) {
                    glLoadName(z);
                    glPushName(0);
                    for (int i = 0; i < 6; ++i) {
                        glLoadName(i);
                        t->init();
                        Tile::rock->renderFaceNoTexture(t, x, y, z, i);
                        t->flush();
                    }
                    glPopName();
                }
            }
            glPopName();
        }
        glPopName();
    }
    glPopName();
    glPopName();
}

void LevelRenderer::renderHit(unique_ptr<HitResult>& h) {
    std::shared_ptr<Tesselator> t = Tesselator::instance;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 1.0f, 1.0f, (float)(((float)std::sin(((double)Timer::nanoTime() / 1000000.0) / 100.0) * 0.2f + 0.4f) * 0.5f));
    t->init();
    Tile::rock->renderFaceNoTexture(t, h->x, h->y, h->z, h->face);
    t->flush();
    glDisable(GL_BLEND);
}

void LevelRenderer::setDirty(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ) {
    minX /= CHUNK_SIZE; maxX /= CHUNK_SIZE;
    minY /= CHUNK_SIZE; maxY /= CHUNK_SIZE;
    minZ /= CHUNK_SIZE; maxZ /= CHUNK_SIZE;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (minZ < 0) minZ = 0;

    if (maxX >= xChunks) maxX = xChunks - 1;
    if (maxY >= yChunks) maxY = yChunks - 1;
    if (maxZ >= zChunks) maxZ = zChunks - 1;

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                Chunk* c = chunks[(x + y * xChunks) * zChunks + z];
                if (c) c->setDirty();
            }
        }
    }
}

void LevelRenderer::tileChanged(int32_t x, int32_t y, int32_t z) {
    setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void LevelRenderer::lightColumnChanged(int32_t x, int32_t z, int32_t minY, int32_t maxY) {
    setDirty(x - 1, minY - 1, z - 1, x + 1, maxY + 1, z + 1);
}

void LevelRenderer::allChanged() {
    setDirty(0, 0, 0, level->width, level->depth, level->height);
}