#include "level/tile/GlassTile.h"

GlassTile::GlassTile(int32_t id, int32_t tex, bool var3) 
    : Tile(id, tex) {}

bool GlassTile::isSolid() {
    return false;
}

bool GlassTile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) {
    int32_t tile = level->getTile(x, y, z);
    return !renderAdjacentFaces && tile == id ? false : Tile::shouldRenderFace(level, x, y, z, layer, var6);
}

bool GlassTile::blocksLight() {
    return false;
}