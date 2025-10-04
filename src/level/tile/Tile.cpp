#include <iostream>
#include "level/tile/Tile.h"
#include "level/tile/DirtTile.h"
#include "level/tile/GrassTile.h"
#include "level/tile/Bush.h"

vector<Tile*> Tile::tiles = vector<Tile*>(256);
Tile* Tile::rock = new Tile(1, 1);
Tile* Tile::grass = new GrassTile(2);
Tile* Tile::dirt = new DirtTile(3, 2);
Tile* Tile::stoneBrick = new Tile(4, 16);
Tile* Tile::wood = new Tile(5, 4);
Tile* Tile::bush = new Bush(6);

Tile::Tile(int32_t id) : id(id) {
    tiles[id] = this;
}

Tile::Tile(int32_t id, int32_t tex) : id(id), tex(tex) {
    tiles[id] = this;
}

void Tile::render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) {
    float c1 = 1.0f;
    float c2 = 0.8f;
    float c3 = 0.6f;
    if (this->shouldRenderFace(level, x, y - 1, z, layer)) {
        t->color(c1, c1, c1);
        this->renderFace(t, x, y, z, 0);
    }
    if (this->shouldRenderFace(level, x, y + 1, z, layer)) {
        t->color(c1, c1, c1);
        this->renderFace(t, x, y, z, 1);
    }
    if (this->shouldRenderFace(level, x, y, z - 1, layer)) {
        t->color(c2, c2, c2);
        this->renderFace(t, x, y, z, 2);
    }
    if (this->shouldRenderFace(level, x, y, z + 1, layer)) {
        t->color(c2, c2, c2);
        this->renderFace(t, x, y, z, 3);
    }
    if (this->shouldRenderFace(level, x - 1, y, z, layer)) {
        t->color(c3, c3, c3);
        this->renderFace(t, x, y, z, 4);
    }
    if (this->shouldRenderFace(level, x + 1, y, z, layer)) {
        t->color(c3, c3, c3);
        this->renderFace(t, x, y, z, 5);
    }
}

bool Tile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer) {
    return !level->isSolidTile(x, y, z) && level->isLit(x, y, z) ^ layer == 1;
}

int32_t Tile::getTexture(int32_t face) {
    return this->tex;
}

void Tile::renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) {
    int32_t tex = this->getTexture(face);
    float minU = (float)(tex % 16) / 16.0f;
    float maxU = minU + 0.0624375f;
    float minV = (float)(tex / 16) / 16.0f;
    float maxV = minV + 0.0624375f;
    float minX = (float)x;
    float maxX = (float)x + 1.0f;
    float minY = (float)y;
    float maxY = (float)y + 1.0f;
    float minZ = (float)z;
    float maxZ = (float)z + 1.0f;
    if (face == 0) {
        t->vertexUV(minX, minY, maxZ, minU, maxV);
        t->vertexUV(minX, minY, minZ, minU, minV);
        t->vertexUV(maxX, minY, minZ, maxU, minV);
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
    }
    if (face == 1) {
        t->vertexUV(maxX, maxY, maxZ, maxU, maxV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(minX, maxY, maxZ, minU, maxV);
    }
    if (face == 2) {
        t->vertexUV(minX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, maxY, minZ, minU, minV);
        t->vertexUV(maxX, minY, minZ, minU, maxV);
        t->vertexUV(minX, minY, minZ, maxU, maxV);
    }
    if (face == 3) {
        t->vertexUV(minX, maxY, maxZ, minU, minV);
        t->vertexUV(minX, minY, maxZ, minU, maxV);
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
        t->vertexUV(maxX, maxY, maxZ, maxU, minV);
    }
    if (face == 4) {
        t->vertexUV(minX, maxY, maxZ, maxU, minV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(minX, minY, minZ, minU, maxV);
        t->vertexUV(minX, minY, maxZ, maxU, maxV);
    }
    if (face == 5) {
        t->vertexUV(maxX, minY, maxZ, minU, maxV);
        t->vertexUV(maxX, minY, minZ, maxU, maxV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, maxY, maxZ, minU, minV);
    }
}

void Tile::renderFaceNoTexture(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) {
    float minX = x;
    float maxX = (float)x + 1.0f;
    float minY = (float)y;
    float maxY = (float)y + 1.0f;
    float minZ = (float)z;
    float maxZ = (float)z + 1.0f;

    if (face == 0) {
        t->vertex(minX, minY, maxZ); t->vertex(minX, minY, minZ); t->vertex(maxX, minY, minZ); t->vertex(maxX, minY, maxZ);
    }
    if (face == 1) {
        t->vertex(maxX, maxY, maxZ); t->vertex(maxX, maxY, minZ); t->vertex(minX, maxY, minZ); t->vertex(minX, maxY, maxZ);
    }
    if (face == 2) {
        t->vertex(minX, maxY, minZ); t->vertex(maxX, maxY, minZ); t->vertex(maxX, minY, minZ); t->vertex(minX, minY, minZ);
    }
    if (face == 3) {
        t->vertex(minX, maxY, maxZ); t->vertex(minX, minY, maxZ); t->vertex(maxX, minY, maxZ); t->vertex(maxX, maxY, maxZ);
    }
    if (face == 4) {
        t->vertex(minX, maxY, maxZ); t->vertex(minX, maxY, minZ); t->vertex(minX, minY, minZ); t->vertex(minX, minY, maxZ);
    }
    if (face == 5) {
        t->vertex(maxX, minY, maxZ); t->vertex(maxX, minY, minZ); t->vertex(maxX, maxY, minZ); t->vertex(maxX, maxY, maxZ);
    }
}

shared_ptr<AABB> Tile::getTileAABB(int32_t x, int32_t y, int32_t z) const {
    return make_shared<AABB>(x, y, z, x + 1, y + 1, z + 1);
}

shared_ptr<AABB> Tile::getAABB(int32_t x, int32_t y, int32_t z) {
    return make_shared<AABB>(x, y, z, x + 1, y + 1, z + 1);
}

bool Tile::blocksLight() {
    return true;
}

bool Tile::isSolid() {
    return true;
}

void Tile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {}

void Tile::destroy(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, shared_ptr<ParticleEngine>& particleEngine) {
    int32_t SD = 4;
    for (int32_t xx = 0; xx < SD; xx++) {
        for (int32_t yy = 0; yy < SD; yy++) {
            for (int32_t zz = 0; zz < SD; zz++) {
                float xp = (float)x + ((float)xx + 0.5f) / (float)SD;
                float yp = (float)y + ((float)yy + 0.5f) / (float)SD;
                float zp = (float)z + ((float)zz + 0.5f) / (float)SD;
                shared_ptr<Particle> particle = make_shared<Particle>(level, xp, yp, zp, xp - (float)x - 0.5f, yp - (float)y - 0.5f, zp - (float)z - 0.5f, this->tex);
                particleEngine->add(particle);
            }
        }
    }
}