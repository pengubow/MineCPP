#include "level/tile/LiquidTile.h"
#include <iostream>
LiquidTile::LiquidTile(int32_t id, int32_t type) 
    : Tile(id) {
    liquidType = type;
    this->tex = 14;
    if (type == 2) {
        this->tex = 30;
    }

    tileId = id;
    calmTileId = id + 1;
    float var3 = 0.1f;
    setShape(0.0f, 0.0f - var3, 0.0f, 1.0f, 1.0f - var3, 1.0f);
    setTicking(true);
}

void LiquidTile::onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    level->addToTickNextTick(x, y, z, tileId);
}

void LiquidTile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    bool var7 = false;
    bool var9 = false;

    bool var6;
    do {
        --y;
        if(level->getTile(x, y, z) != 0) {
            break;
        }

        var6 = level->setTile(x, y, z, tileId);
        if(var6) {
            var9 = true;
        }
    } while (var6 && liquidType != 2);

    y++;
    if (liquidType == 1 || !var9) {
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

bool LiquidTile::checkWater(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    int32_t var5 = level->getTile(x, y, z);
    if (var5 == 0) {
        bool var6 = level->setTile(x, y, z, tileId);
        if (var6) {
            level->addToTickNextTick(x, y, z, tileId);
        }
    }

    return false;
}

bool LiquidTile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) {
    if (x >= 0 && y >= 0 && z >= 0 && x < level->width && z < level->height) {
        if (layer != 1 && liquidType == 1) {
            return false;
        }
        else {
            int32_t tile = level->getTile(x, y, z);
            return tile != tileId && tile != calmTileId ? Tile::shouldRenderFace(level, x, y, z, -1, var6) : false;
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

bool LiquidTile::mayPick() {
    return false;
}

shared_ptr<AABB> LiquidTile::getAABB(int32_t x, int32_t y, int32_t z) {
    return nullptr;
}

bool LiquidTile::blocksLight() {
    return true;
}

bool LiquidTile::isSolid() {
    return false;
}

int32_t LiquidTile::getLiquidType() {
    return liquidType;
}

void LiquidTile::neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) {
    if (liquidType == 1 && (type == Tile::lava->id || type == Tile::calmLava->id)) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }

    if (liquidType == 2 && (type == Tile::water->id || type == Tile::calmWater->id)) {
        level->setTileNoUpdate(x, y, z, Tile::rock->id);
    }

    level->addToTickNextTick(x, y, z, type);
}