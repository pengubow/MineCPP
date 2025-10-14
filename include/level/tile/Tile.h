#pragma once

#include <memory>
#include "renderer/Tesselator.h"
#include "level/Level.h"
#include "particle/ParticleEngine.h"

class Tile {
public:
    static vector<Tile*> tiles;
    static vector<bool> shouldTick;
    static Tile* empty;
    static Tile* rock;
    static Tile* grass;
    static Tile* dirt;
    static Tile* stoneBrick;
    static Tile* wood;
    static Tile* bush;
    static Tile* unbreakable;
    static Tile* water;
    static Tile* calmWater;
    static Tile* lava;
    static Tile* calmLava;
    static Tile* sand;
    static Tile* gravel;
    static Tile* oreGold;
    static Tile* oreIron;
    static Tile* oreCoal;
    static Tile* log;
    static Tile* leaf;
    int32_t tex;
    int32_t id;
private:
    float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;
protected:
    Tile(int32_t tex);

    void setTicking(bool shouldTick);
    void setShape(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

    Tile(int32_t id, int32_t tex);
public:
    virtual bool render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z);
    static bool cullFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t face);
protected:
    virtual bool shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6);
protected:
    virtual int32_t getTexture(int32_t face);
public:
    virtual void renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face);
    virtual void renderBackFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face);
    static void renderFaceNoTexture(shared_ptr<Player>& player, shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t var5);
    static shared_ptr<AABB> getTileAABB(int32_t x, int32_t y, int32_t z);
    virtual shared_ptr<AABB> getAABB(int32_t x, int32_t y, int32_t z);
    virtual bool blocksLight();
    virtual bool isSolid();
    virtual bool mayPick();
    virtual void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
    void destroy(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, shared_ptr<ParticleEngine>& particleEngine);
    virtual int32_t getLiquidType();
    virtual void neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type);
    virtual void onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
};