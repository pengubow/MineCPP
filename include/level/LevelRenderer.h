#pragma once

#include <vector>
#include <memory>
#include "level/Level.h"
#include "level/Chunk.h"
#include "renderer/Tesselator.h"
#include "renderer/Frustum.h"
#include "Player.h"
#include "level/LevelListener.h"
#include "renderer/Textures.h"
#include "HitResult.h"

class LevelRenderer : public LevelListener {
public:
    static const int32_t MAX_REBUILDS_PER_FRAME = 8;
    static const int32_t CHUNK_SIZE = 16;
private:
    shared_ptr<Level> level;
    vector<Chunk*> chunks;
    int32_t xChunks;
    int32_t yChunks;
    int32_t zChunks;
    shared_ptr<Textures> textures;
    float prevalpha;
public:
    LevelRenderer(shared_ptr<Level>& level, shared_ptr<Textures>& textures);

    vector<Chunk*> getAllDirtyChunks();
    void render(shared_ptr<Player>& player, int32_t layer);
    void updateDirtyChunks(shared_ptr<Player> player);
    void pick(shared_ptr<Player>& player, Frustum& frustum);
    void renderHit(unique_ptr<HitResult>& h, int32_t mode, int32_t tileType);
    void setDirty(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);
    void tileChanged(int32_t x, int32_t y, int32_t z) override;
    void lightColumnChanged(int32_t x, int32_t z, int32_t minY, int32_t maxY) override;
    void allChanged() override;
};