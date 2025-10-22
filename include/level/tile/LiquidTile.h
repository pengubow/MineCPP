#pragma once

#include "level/tile/Tile.h"
#include "level/liquid/Liquid.h"

class LiquidTile : public Tile {
protected:
    Liquid* liquid;
    int32_t calmTileId;
    int32_t tileId;
public:
    LiquidTile(int32_t id, Liquid* type);

    void onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
private:
    bool checkWater(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
protected:
    float getBrightness(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    bool shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) override;
public:
    void renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) override;
    bool mayPick() override;
    optional<AABB> getAABB(int32_t x, int32_t y, int32_t z) override;
    bool blocksLight() override;
    bool isSolid() override;
    Liquid* getLiquidType() override;
    void neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) override;
    int32_t getTickDelay() override;
};