#include <iostream>
#include "level/tile/Tile.h"
#include "level/tile/DirtTile.h"
#include "level/tile/GrassTile.h"
#include "level/tile/Bush.h"
#include "level/tile/LiquidTile.h"
#include "level/tile/CalmLiquidTile.h"
#include "level/tile/FallingTile.h"
#include "level/tile/LogTile.h"
#include "level/tile/LeafTile.h"
#include "level/tile/SpongeTile.h"
#include "level/tile/GlassTile.h"

Random Tile::random = Random();

const map<Tile::SoundType, Tile::SoundTypeData> Tile::soundTypeMap = {
    {SoundType::NONE,    {"-",       0.0f, 0.0f}},
    {SoundType::GRASS,   {"grass",   0.6f, 1.0f}},
    {SoundType::CLOTH,   {"grass",   0.7f, 1.2f}},
    {SoundType::GRAVEL,  {"gravel",  1.0f, 1.0f}},
    {SoundType::STONE,   {"stone",   1.0f, 1.0f}},
    {SoundType::METAL,   {"stone",   1.0f, 2.0f}},
    {SoundType::WOOD,    {"wood",    1.0f, 1.0f}}
};

vector<Tile*> Tile::tiles = vector<Tile*>(256);
vector<bool> Tile::shouldTick = vector<bool>(256);
vector<int32_t> Tile::tickSpeed = vector<int32_t>(256);
Tile* Tile::rock = (new Tile(1, 1))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::grass = (new GrassTile(2))->setSoundAndGravity(SoundType::GRASS, 0.9f, 1.0f);
Tile* Tile::dirt = (new DirtTile(3, 2))->setSoundAndGravity(SoundType::GRASS, 0.8f, 1.0f);
Tile* Tile::wood = (new Tile(4, 16))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::stoneBrick = (new Tile(5, 4))->setSoundAndGravity(SoundType::WOOD, 1.0f, 1.0f);
Tile* Tile::bush = (new Bush(6, 15))->setSoundAndGravity(SoundType::NONE, 0.7f, 1.0f);
Tile* Tile::unbreakable = (new Tile(7, 17))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::water = (new LiquidTile(8, Liquid::water))->setSoundAndGravity(SoundType::NONE, 1.0f, 1.0f);
Tile* Tile::calmWater = (new CalmLiquidTile(9, Liquid::water))->setSoundAndGravity(SoundType::NONE, 1.0f, 1.0f);
Tile* Tile::lava = (new LiquidTile(10, Liquid::lava))->setSoundAndGravity(SoundType::NONE, 1.0f, 1.0f);
Tile* Tile::calmLava = (new CalmLiquidTile(11, Liquid::lava))->setSoundAndGravity(SoundType::NONE, 1.0f, 1.0f);
Tile* Tile::sand = (new FallingTile(12, 18))->setSoundAndGravity(SoundType::GRAVEL, 0.8f, 1.0f);
Tile* Tile::gravel = (new FallingTile(13, 19))->setSoundAndGravity(SoundType::GRAVEL, 0.8f, 1.0f);
Tile* Tile::oreGold = (new Tile(14, 32))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::oreIron = (new Tile(15, 33))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::oreCoal = (new Tile(16, 34))->setSoundAndGravity(SoundType::STONE, 1.0f, 1.0f);
Tile* Tile::log = (new LogTile(17))->setSoundAndGravity(SoundType::WOOD, 1.0f, 1.0f);
Tile* Tile::leaf = (new LeafTile(18, 22, true))->setSoundAndGravity(SoundType::GRASS, 1.0f, 0.4f);
Tile* Tile::sponge = (new SpongeTile(19))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 0.9f);
Tile* Tile::glass = (new GlassTile(20, 49, false))->setSoundAndGravity(SoundType::METAL, 1.0f, 1.0f);
Tile* Tile::clothRed = (new Tile(21, 64))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothOrange = (new Tile(22, 65))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothYellow = (new Tile(23, 66))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothChartreuse = (new Tile(24, 67))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothGreen = (new Tile(25, 68))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothSpringGreen = (new Tile(26, 69))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothCyan = (new Tile(27, 70))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothCapri = (new Tile(28, 71))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothUltramarine = (new Tile(29, 72))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothViolet = (new Tile(30, 73))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothPurple = (new Tile(31, 74))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothMagenta = (new Tile(32, 75))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothRose = (new Tile(33, 76))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothDarkGray = (new Tile(34, 77))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothGray = (new Tile(35, 78))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::clothWhite = (new Tile(36, 79))->setSoundAndGravity(SoundType::CLOTH, 1.0f, 1.0f);
Tile* Tile::plantYellow = (new Bush(37, 13))->setSoundAndGravity(SoundType::NONE, 0.7f, 1.0f);
Tile* Tile::plantRed = (new Bush(38, 12))->setSoundAndGravity(SoundType::NONE, 0.7f, 1.0f);
Tile* Tile::mushroomBrown = (new Bush(39, 29))->setSoundAndGravity(SoundType::NONE, 0.7f, 1.0f);
Tile* Tile::mushroomRed = (new Bush(40, 28))->setSoundAndGravity(SoundType::NONE, 0.7f, 1.0f);
Tile* Tile::blockGold = (new Tile(41, 40))->setSoundAndGravity(SoundType::METAL, 0.7f, 1.0f);

Tile::Tile(int32_t id) : id(id) {
    tiles[id] = this;
    setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

Tile* Tile::setSoundAndGravity(SoundType soundType, float var2, float particleGravity) {
    this->particleGravity = particleGravity;
    this->soundType = soundType;
    return this;
}

void Tile::setTickSpeed(int32_t tickSpeed) {
    this->tickSpeed[id] = tickSpeed;
}

void Tile::setTicking(bool shouldTick) {
    this->shouldTick[id] = shouldTick;
}

void Tile::setShape(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    this->minX = minX;
    this->minY = minY;
    this->minZ = minZ;
    this->maxX = maxX;
    this->maxY = maxY;
    this->maxZ = maxZ;
}

Tile::Tile(int32_t id, int32_t tex) : id(id), tex(tex) {
    tiles[id] = this;
    setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

bool Tile::render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) {
    bool returnValue = false;
    float var8 = 0.5f;
    float c1;
    float c2 = 0.8f;
    float c3 = 0.6f;
    if (this->shouldRenderFace(level, x, y - 1, z, layer, 0)) {
        c1 = getBrightness(level, x, y - 1, z);
        t->color(var8 * c1, var8 * c1, var8 * c1);
        this->renderFace(t, x, y, z, 0);
        returnValue = true;
    }
    if (this->shouldRenderFace(level, x, y + 1, z, layer, 1)) {
        c1 = getBrightness(level, x, y + 1, z);
        t->color(c1, c1, c1);
        this->renderFace(t, x, y, z, 1);
        returnValue = true;
    }
    if (this->shouldRenderFace(level, x, y, z - 1, layer, 2)) {
        c1 = getBrightness(level, x, y, z - 1);
        t->color(c1 * c2, c1 * c2, c1 * c2);
        this->renderFace(t, x, y, z, 2);
        returnValue = true;
    }
    if (this->shouldRenderFace(level, x, y, z + 1, layer, 3)) {
        c1 = getBrightness(level, x, y, z + 1);
        t->color(c1 * c2, c1 * c2, c1 * c2);
        this->renderFace(t, x, y, z, 3);
        returnValue = true;
    }
    if (this->shouldRenderFace(level, x - 1, y, z, layer, 4)) {
        c1 = getBrightness(level, x - 1, y, z);
        t->color(c1 * c3, c1 * c3, c1 * c3);
        this->renderFace(t, x, y, z, 4);
        returnValue = true;
    }
    if (this->shouldRenderFace(level, x + 1, y, z, layer, 5)) {
        c1 = getBrightness(level, x + 1, y, z);
        t->color(c1 * c3, c1 * c3, c1 * c3);
        this->renderFace(t, x, y, z, 5);
        returnValue = true;
    }
    return returnValue;
}

void Tile::onTileAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {}

void Tile::onTileRemoved(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {}

float Tile::getBrightness(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {
    return level->getBrightness(x, y, z);
}

bool Tile::shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) {
    return layer == 1 ? false : !level->isSolidTile(x, y, z);
}

int32_t Tile::getTexture(int32_t face) {
    return this->tex;
}

void Tile::renderFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) {
    int32_t tex = getTexture(face);
    int32_t baseU = (tex % 16) * 16;
    int32_t baseV = (tex / 16) * 16;
    float minU = (float)baseU / 256.0f;
    float maxU = ((float)baseU + 15.99f) / 256.0f;
    float minV = (float)baseV / 256.0f;
    float maxV = ((float)baseV + 15.99f) / 256.0f;
    float minX = (float)x + this->minX;
    float maxX = (float)x + this->maxX;
    float minY = (float)y + this->minY;
    float maxY = (float)y + this->maxY;
    float minZ = (float)z + this->minZ;
    float maxZ = (float)z + this->maxZ;
    if (face == 0) {
        t->vertexUV(minX, minY, maxZ, minU, maxV);
        t->vertexUV(minX, minY, minZ, minU, minV);
        t->vertexUV(maxX, minY, minZ, maxU, minV);
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
    }
    else if (face == 1) {
        t->vertexUV(maxX, maxY, maxZ, maxU, maxV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(minX, maxY, maxZ, minU, maxV);
    }
    else if (face == 2) {
        t->vertexUV(minX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, maxY, minZ, minU, minV);
        t->vertexUV(maxX, minY, minZ, minU, maxV);
        t->vertexUV(minX, minY, minZ, maxU, maxV);
    }
    else if (face == 3) {
        t->vertexUV(minX, maxY, maxZ, minU, minV);
        t->vertexUV(minX, minY, maxZ, minU, maxV);
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
        t->vertexUV(maxX, maxY, maxZ, maxU, minV);
    }
    else if (face == 4) {
        t->vertexUV(minX, maxY, maxZ, maxU, minV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(minX, minY, minZ, minU, maxV);
        t->vertexUV(minX, minY, maxZ, maxU, maxV);
    }
    else if (face == 5) {
        t->vertexUV(maxX, minY, maxZ, minU, maxV);
        t->vertexUV(maxX, minY, minZ, maxU, maxV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, maxY, maxZ, minU, minV);
    }
}

void Tile::renderBackFace(shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t face) {
    int32_t tex = getTexture(face);
    float minU = (float)(tex % 16) / 16.0F;
    float maxU = minU + 0.999F / 16.0F;
    float minV = (float)(tex / 16) / 16.0F;
    float maxV = minV + 0.999F / 16.0F;
    float minX = (float)x + this->minX;
    float maxX = (float)x + this->maxX;
    float minY = (float)y + this->minY;
    float maxY = (float)y + this->maxY;
    float minZ = (float)z + this->minZ;
    float maxZ = (float)z + this->maxZ;
    if (face == 0) {
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
        t->vertexUV(maxX, minY, minZ, maxU, minV);
        t->vertexUV(minX, minY, minZ, minU, minV);
        t->vertexUV(minX, minY, maxZ, minU, maxV);
    }

    if (face == 1) {
        t->vertexUV(minX, maxY, maxZ, minU, maxV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, maxY, maxZ, maxU, maxV);
    }

    if (face == 2) {
        t->vertexUV(minX, minY, minZ, maxU, maxV);
        t->vertexUV(maxX, minY, minZ, minU, maxV);
        t->vertexUV(maxX, maxY, minZ, minU, minV);
        t->vertexUV(minX, maxY, minZ, maxU, minV);
    }

    if (face == 3) {
        t->vertexUV(maxX, maxY, maxZ, maxU, minV);
        t->vertexUV(maxX, minY, maxZ, maxU, maxV);
        t->vertexUV(minX, minY, maxZ, minU, maxV);
        t->vertexUV(minX, maxY, maxZ, minU, minV);
    }

    if (face == 4) {
        t->vertexUV(minX, minY, maxZ, maxU, maxV);
        t->vertexUV(minX, minY, minZ, minU, maxV);
        t->vertexUV(minX, maxY, minZ, minU, minV);
        t->vertexUV(minX, maxY, maxZ, maxU, minV);
    }

    if (face == 5) {
        t->vertexUV(maxX, maxY, maxZ, minU, minV);
        t->vertexUV(maxX, maxY, minZ, maxU, minV);
        t->vertexUV(maxX, minY, minZ, maxU, maxV);
        t->vertexUV(maxX, minY, maxZ, minU, maxV);
    }
}

void Tile::renderFaceNoTexture(shared_ptr<Entity> entity, shared_ptr<Tesselator>& t, int32_t x, int32_t y, int32_t z, int32_t var5) {
    float minX = (float)x;
    float maxX = (float)x + 1.0f;
    float minY = (float)y;
    float maxY = (float)y + 1.0f;
    float minZ = (float)z;
    float maxZ = (float)z + 1.0f;
    if(var5 == 0 && (float)y > entity->y) {
        t->vertex(minX, minY, maxZ);
        t->vertex(minX, minY, minZ);
        t->vertex(maxX, minY, minZ);
        t->vertex(maxX, minY, maxZ);
    }

    if(var5 == 1 && (float)y < entity->y) {
        t->vertex(maxX, maxY, maxZ);
        t->vertex(maxX, maxY, minZ);
        t->vertex(minX, maxY, minZ);
        t->vertex(minX, maxY, maxZ);
    }

    if(var5 == 2 && (float)z > entity->z) {
        t->vertex(minX, maxY, minZ);
        t->vertex(maxX, maxY, minZ);
        t->vertex(maxX, minY, minZ);
        t->vertex(minX, minY, minZ);
    }

    if(var5 == 3 && (float)z < entity->z) {
        t->vertex(minX, maxY, maxZ);
        t->vertex(minX, minY, maxZ);
        t->vertex(maxX, minY, maxZ);
        t->vertex(maxX, maxY, maxZ);
    }

    if(var5 == 4 && (float)minX > entity->x) {
        t->vertex(minX, maxY, maxZ);
        t->vertex(minX, maxY, minZ);
        t->vertex(minX, minY, minZ);
        t->vertex(minX, minY, maxZ);
    }

    if(var5 == 5 && (float)minX < entity->x) {
        t->vertex(maxX, minY, maxZ);
        t->vertex(maxX, minY, minZ);
        t->vertex(maxX, maxY, minZ);
        t->vertex(maxX, maxY, maxZ);
    }

}

optional<AABB> Tile::getTileAABB(int32_t x, int32_t y, int32_t z) {
    return AABB(x, y, z, x + 1, y + 1, z + 1);
}

bool Tile::blocksLight() {
    return true;
}

bool Tile::isSolid() {
    return true;
}

void Tile::tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, Random& random) {}

void Tile::destroy(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, shared_ptr<ParticleEngine>& particleEngine) {
    int32_t SD = 4;
    for (int32_t xx = 0; xx < SD; xx++) {
        for (int32_t yy = 0; yy < SD; yy++) {
            for (int32_t zz = 0; zz < SD; zz++) {
                float xp = (float)x + ((float)xx + 0.5f) / (float)SD;
                float yp = (float)y + ((float)yy + 0.5f) / (float)SD;
                float zp = (float)z + ((float)zz + 0.5f) / (float)SD;
                shared_ptr<Particle> particle = make_shared<Particle>(level, xp, yp, zp, xp - (float)x - 0.5f, yp - (float)y - 0.5f, zp - (float)z - 0.5f, this);
                particleEngine->particles.push_back(particle);
            }
        }
    }
}

Liquid* Tile::getLiquidType() {
    return Liquid::none;
}

void Tile::neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) {}

void Tile::onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) {}

int32_t Tile::getTickDelay() {
    return 0;
}