#pragma once

#include <memory>
#include "level/Tesselator.h"
#include "level/Level.h"
#include "particle/ParticleEngine.h"

class Tile {
public:
    static vector<Tile*> tiles;
    static Tile* empty;
    static Tile* rock;
    static Tile* grass;
    static Tile* dirt;
    static Tile* stoneBrick;
    static Tile* wood;
    static Tile* bush;
    int32_t tex;
    const int32_t id;
protected:
    Tile(int32_t tex);

    Tile(int32_t id, int32_t tex);
public:
    virtual void render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z);
private:
    bool shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer);
protected:
    virtual int32_t getTexture(int32_t face);
public:
    void renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face);
    void renderFaceNoTexture(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face);
    shared_ptr<AABB> getTileAABB(int32_t x, int32_t y, int32_t z) const;
    virtual shared_ptr<AABB> getAABB(int32_t x, int32_t y, int32_t z);
    virtual bool blocksLight();
    virtual bool isSolid();
    virtual void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
    void destroy(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, shared_ptr<ParticleEngine>& particleEngine);
};