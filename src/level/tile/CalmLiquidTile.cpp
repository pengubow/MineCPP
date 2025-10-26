#include "level/tile/CalmLiquidTile.h"

CalmLiquidTile::CalmLiquidTile(int32_t id, Liquid* type) 
    : LiquidTile(id, type) {
    tileId = id - 1;
    calmTileId = id;
    setTicking(false);
}

void CalmLiquidTile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, Random& random) {}

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

    if (type != 0) {
        Liquid* liquidType = Tile::tiles[type]->getLiquidType();
        if (liquid == Liquid::water && liquidType == Liquid::lava || liquidType == Liquid::water && liquid == Liquid::lava) {
            level->setTile(x, y, z, Tile::rock->id);
            return;
        }
    }

    if (var6) {
        level->setTileNoUpdate(x, y, z, tileId);
        level->addToTickNextTick(x, y, z, tileId);
    }

}