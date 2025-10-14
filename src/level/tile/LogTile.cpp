#include "level/tile/LogTile.h"

LogTile::LogTile(int32_t tex) 
    : Tile(tex) {
    this->tex = 20;
}

int32_t LogTile::getTexture(int32_t face) {
    return face == 1 ? 21 : (face == 0 ? 21 : 20);
}