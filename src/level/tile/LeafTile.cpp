#include "level/tile/LeafTile.h"

LeafTile::LeafTile(int32_t id, int32_t tex) 
    : Tile(id, tex) {}

bool LeafTile::isSolid() {
    return false;
}

bool LeafTile::blocksLight() {
    return false;
}