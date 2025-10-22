#include "level/tile/LeafTile.h"

LeafTile::LeafTile(int32_t id, int32_t tex) 
    : Tile(id, tex) {
        this->particleGravity = 0.4f;
    }

bool LeafTile::isSolid() {
    return false;
}

bool LeafTile::blocksLight() {
    return false;
}