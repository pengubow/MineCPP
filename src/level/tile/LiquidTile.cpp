#include "level/tile/LiquidTile.h"

LiquidTile::LiquidTile(int32_t id, Liquid* type) 
    : Tile(id) {
    liquid = type;
    this->tex = 14;
    if (type == Liquid::lava) {
        this->tex = 30;
    }

    tileId = id;
    calmTileId = id + 1;
    float var4 = 0.01f;
    float var3 = 0.1f;
    setShape(var4 + 0.0f, 0.0f - var3 + var4, var4 + 0.0f, var4 + 1.0f, 1.0f - var3 + var4, var4 + 1.0f);
    setTicking(true);
    if (type == Liquid::lava) {
        setTickSpeed(16);
    }
}

void LiquidTile::onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    level->addToTickNextTick(x, y, z, tileId);
}

void LiquidTile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, Random& random) {
    bool var7 = false;
    bool var9 = false;

    bool var6;
    do {
        --y;
        if (level->getTile(x, y, z) != 0 || !checkSponge(level, x, y, z)) {
            break;
        }

        var6 = level->setTile(x, y, z, tileId);
        if (var6) {
            var9 = true;
        }
    } while (var6 && liquid != Liquid::lava);

    y++;
    if (liquid == Liquid::water || !var9) {
        var9 |= checkWater(level, x - 1, y, z);
        var9 |= checkWater(level, x + 1, y, z);
        var9 |= checkWater(level, x, y, z - 1);
        var9 |= checkWater(level, x, y, z + 1);
    }

    if (!var9) {
        level->setTileNoUpdate(x, y, z, calmTileId);
    }
    else {
        level->addToTickNextTick(x, y, z, tileId);
    }
}

bool LiquidTile::checkSponge(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    if (liquid == Liquid::water) {
        for (int32_t ix = x - 2; ix <= x + 2; ix++) {
            for (int32_t iy = y - 2; iy <= y + 2; iy++) {
                for (int32_t iz = z - 2; iz <= z + 2; iz++) {
                    if (level->getTile(ix, iy, iz) == Tile::sponge->id) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool LiquidTile::checkWater(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    int32_t var5 = level->getTile(x, y, z);
    if (var5 == 0) {
        if (!checkSponge(level, x, y, z)) {
            return false;
        }

        bool var6 = level->setTile(x, y, z, tileId);
        if (var6) {
            level->addToTickNextTick(x, y, z, tileId);
        }
    }

    return false;
}

float LiquidTile::getBrightness(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    return liquid == Liquid::lava ? 100.0f : level->getBrightness(x, y, z);
}

bool LiquidTile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) {
    if (x >= 0 && y >= 0 && z >= 0 && x < level->width && z < level->height) {
        if (layer != 1 && liquid == Liquid::water) {
            return false;
        }
        else {
            int32_t tile = level->getTile(x, y, z);
            return tile != tileId && tile != calmTileId ? (var6 != 1 || level->getTile(x - 1, y, z) != 0 && level->getTile(x + 1, y, z) != 0 && level->getTile(x, y, z - 1) != 0 && level->getTile(x, y, z + 1) != 0 ? Tile::shouldRenderFace(level, x, y, z, -1, var6) : true) : false;
        }
    }
    else {
        return false;
    }
}

void LiquidTile::renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) {
    Tile::renderFace(t, x, y, z, face);
    renderBackFace(t, x, y, z, face);
}

optional<AABB> LiquidTile::getTileAABB(int32_t x, int32_t y, int32_t z) {
    return nullopt;
}

bool LiquidTile::blocksLight() {
    return true;
}

bool LiquidTile::isSolid() {
    return false;
}

Liquid* LiquidTile::getLiquidType() {
    return liquid;
}

void LiquidTile::neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) {
    if (type != 0) {
        Liquid* liquidType = Tile::tiles[type]->getLiquidType();
        if(liquid == Liquid::water && liquidType == Liquid::lava || liquidType == Liquid::water && liquid == Liquid::lava) {
            level->setTile(x, y, z, Tile::rock->id);
            return;
        }
    }

    level->addToTickNextTick(x, y, z, type);
}

int32_t LiquidTile::getTickDelay() {
    return liquid == Liquid::lava ? 5 : 0;
}