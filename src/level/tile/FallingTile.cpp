#include "level/tile/FallingTile.h"

FallingTile::FallingTile(int32_t id, int32_t tex)
    : Tile(id, tex) {}

void FallingTile::onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    tryToFall(level, x, y, z);
}

void FallingTile::neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) {
    tryToFall(level, x, y, z);
}

void FallingTile::tryToFall(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    int32_t var4 = x;
    int32_t var5 = y;

    int32_t var6;
    for(var6 = z; level->getTile(var4, var5 - 1, var6) == 0 && var5 > 0; --var5) {}

    if(var5 != y) {
        level->swap(x, y, z, var4, var5, var6);
    }
}