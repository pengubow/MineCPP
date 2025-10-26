#include "level/tile/LeafTile.h"

LeafTile::LeafTile(int32_t id, int32_t tex, bool var3) 
    : Tile(id, tex) {
    this->particleGravity = 0.4f;
}

bool LeafTile::isSolid() {
    return false;
}

bool LeafTile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) {
    int32_t tile = level->getTile(x, y, z);
    return !renderAdjacentFaces && tile == id ? false : Tile::shouldRenderFace(level, x, y, z, layer, var6);
}

bool LeafTile::blocksLight() {
    return false;
}