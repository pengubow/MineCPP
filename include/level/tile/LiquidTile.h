#pragma once

#include "level/tile/Tile.h"

class LiquidTile : public Tile {
protected:
    int32_t liquidType;
    int32_t calmTileId;
    int32_t tileId;
public:
    LiquidTile(int32_t id, int32_t type);

    void onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
private:
    bool checkWater(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
protected:
    bool shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) override;
public:
    void renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) override;
    bool mayPick() override;
    shared_ptr<AABB> getAABB(int32_t x, int32_t y, int32_t z) override;
    bool blocksLight() override;
    bool isSolid() override;
    int32_t getLiquidType() override;
    void neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) override;
};