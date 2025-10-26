#include "level/tile/SpongeTile.h"

SpongeTile::SpongeTile(int32_t id) 
    : Tile(id) {
    tex = 48;
    particleGravity = 0.9f;
}

void SpongeTile::onTileAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    for (int32_t ix = x - 2; ix <= x + 2; ix++) {
        for (int32_t iy = y - 2; iy <= y + 2; iy++) {
            for (int32_t iz = z - 2; iz <= z + 2; iz++) {
                if (level->isWater(ix, iy, iz)) {
                    level->setTileNoNeighborChange(ix, iy, iz, 0);
                }
            }
        }
    }
}

void SpongeTile::onTileRemoved(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    for (int32_t ix = x - 2; ix <= x + 2; ix++) {
        for (int32_t iy = y - 2; iy <= y + 2; iy++) {
            for (int32_t iz = z - 2; iz <= z + 2; iz++) {
                level->updateNeighborsAt(ix, iy, iz, level->getTile(ix, iy, iz));
            }
        }
    }
}