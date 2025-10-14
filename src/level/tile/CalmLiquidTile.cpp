#include "level/tile/CalmLiquidTile.h"

CalmLiquidTile::CalmLiquidTile(int32_t id, int32_t tex) 
    : LiquidTile(id, tex) {
    tileId = id - 1;
    calmTileId = id;
    setTicking(false);
}

void CalmLiquidTile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {}

void CalmLiquidTile::neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) {
    bool var6 = false;
    if (level->getTile(x - 1, y, z) == 0) {
        var6 = true;
    }

    if (level->getTile(x + 1, y, z) == 0) {
        var6 = true;
    }

    if (level->getTile(x, y, z - 1) == 0) {
        var6 = true;
    }

    if (level->getTile(x, y, z + 1) == 0) {
        var6 = true;
    }

    if (level->getTile(x, y - 1, z) == 0) {
        var6 = true;
    }

    if (liquidType == 1 && type == Tile::lava->id) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }
    else if (liquidType == 2 && type == Tile::water->id) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }
    else {
        if (var6) {
            level->setTileNoUpdate(x, y, z, tileId);
            level->addToTickNextTick(x, y, z, tileId);
        }

    }
}