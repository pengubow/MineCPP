#include "level/tile/GrassTile.h"
#include "Util.h"

GrassTile::GrassTile(int32_t id) : Tile(id) {
    this->tex = 3;
}

int32_t GrassTile::getTexture(int32_t face) {
    if (face == 1) {
        return 0;
    }
    if (face == 0) {
        return 2;
    }
    return 3;
}

void GrassTile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    if (!level->isLit(x, y, z)) {
        level->setTile(x, y, z, Tile::dirt->id);
    }
    else {
        for (int32_t i = 0; i < 4; i++) {
            int32_t zt = z + Util::nextInt(3) - 1;
            int32_t yt = y + Util::nextInt(5) - 3;
            int32_t xt = x + Util::nextInt(3) - 1;
            if (level->getTile(xt, yt, zt) == Tile::dirt->id && level->isLit(xt, yt, zt)) {
                level->setTile(xt, yt, zt, Tile::grass->id);
            }
        }
    }
}