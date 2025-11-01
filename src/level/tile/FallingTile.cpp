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
    int32_t y2 = y;

    while (true) {
        int32_t var9 = y2 - 1;
        int32_t tile = level->getTile(x, var9, z);
        bool var10000;
        if (tile == 0) {
            var10000 = true;
        }
        else {
            Liquid* liquid = Tile::tiles[tile]->getLiquidType();
            var10000 = liquid == Liquid::water ? true : liquid == Liquid::lava;
        }

        if (!var10000 || y2 <= 0) {
            if (y2 != y) {
                level->swap(x, y, z, x, y2, z);
            }

            return;
        }

        --y2;
    }
}