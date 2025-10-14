#include <unordered_set>
#include "level/Level.h"
#include "renderer/Textures.h"
#include "renderer/Chunk.h"
#include "HitResult.h"

class LevelRenderer : public enable_shared_from_this<LevelRenderer> {
public:
    weak_ptr<Level> level;
private:
    shared_ptr<Textures> textures;
public:
    int32_t surroundLists;
    int32_t drawDistance = 0;
private:
    vector<int32_t> dummyBuffer = vector<int32_t>(65536);
public:
    vector<shared_ptr<Chunk>> dirtyChunks;
private:
    vector<shared_ptr<Chunk>> chunks;
public:
    vector<shared_ptr<Chunk>> sortedChunks;
private:
    int32_t xChunks;
    int32_t yChunks;
    int32_t zChunks;
public:
    int32_t cloudTickCounter = 0;
private:
    float lX = -9999.9f;
    float lY = -9999.9f;
    float lZ = -9999.9f;
public:
    LevelRenderer(shared_ptr<Textures>& textures);

    void setLevel(shared_ptr<Level>& level);
    void compileSurroundingGround();
    void renderEntities(Frustum& frustum, float var2);
    void render(shared_ptr<Player> player, int32_t var2);
    void renderClouds(float var1);
    void render(int32_t x, int32_t y, int32_t z);
    void renderHit(shared_ptr<Player>& player, shared_ptr<HitResult>& h, int32_t mode, int32_t tileType);
    static void renderHitOutline(shared_ptr<HitResult>& h, int32_t mode);
    void setDirty(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ);
};