#include <unordered_set>
#include "level/Level.h"
#include "renderer/Textures.h"
#include "renderer/Chunk.h"
#include "HitResult.h"

class LevelRenderer : public enable_shared_from_this<LevelRenderer> {
public:
    weak_ptr<Level> level;
    shared_ptr<Textures> textures;
    int32_t surroundLists;
    int32_t drawDistance = 0;
    vector<int32_t> dummyBuffer = vector<int32_t>(65536);
    unordered_set<shared_ptr<Chunk>> dirtyChunks;
    vector<shared_ptr<Chunk>> chunks;
    vector<shared_ptr<Chunk>> sortedChunks;
private:
    int32_t xChunks;
    int32_t yChunks;
    int32_t zChunks;
    int32_t chunkRenderLists;
public:
    int32_t cloudTickCounter = 0;
private:
    float lX = -9999.9f;
    float lY = -9999.9f;
    float lZ = -9999.9f;
public:
    LevelRenderer(shared_ptr<Textures>& textures);

    void compileSurroundingGround();
    void renderEntities(Frustum& frustum, float var2);
    int32_t render(shared_ptr<Player>& player, int32_t var2);
    void renderClouds(float var1);
    void render(int32_t x, int32_t y, int32_t z);
    void renderHit(shared_ptr<Player>& player, optional<HitResult>& h, int32_t mode, int32_t tileType);
    static void renderHitOutline(optional<HitResult>& h, int32_t mode);
    void setDirty(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);
};