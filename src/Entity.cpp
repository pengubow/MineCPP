#include "Entity.h"
#include "Util/Util.h"
#include <iostream>

Entity::Entity(shared_ptr<Level>& level) {
    this->level = level;
    this->setPos(0.0f, 0.0f, 0.0f);
}

void Entity::resetPos() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }
    float x = (float)(level->xSpawn) + 0.5f;
    float y = (float)(level->ySpawn);
    for (float z = (float)(level->zSpawn) + 0.5f; y > 0.0f; ++y) {
        setPos(x, y, z);
        if (level->getCubes(bb).size() == 0) {
            break;
        }
    }

    xd = yd = zd = 0.0f;
    yRot = level->rotSpawn;
    xRot = 0.0f;
}

void Entity::remove() {
    this->removed = true;
}

void Entity::setSize(float w, float h) {
    this->bbWidth = w;
    this->bbHeight = h;
}

void Entity::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    float w = this->bbWidth / 2.0f;
    float h = this->bbHeight / 2.0f;
    this->bb = make_shared<AABB>(x - w, y - h, z - w, x + w, y + h, z + w);
}

void Entity::turn(float xo, float yo) {
    this->yRot = (float)((double)this->yRot + (double)xo * 0.15);
    this->xRot = (float)((double)this->xRot - (double)yo * 0.15);
    if (this->xRot < -90.0f) {
        this->xRot = -90.0f;
    }
    if (this->xRot > 90.0f) {
        this->xRot = 90.0f;
    }
}

void Entity::interpolateTurn(float xo, float yo) {
    float xRot = this->xRot;
    float yRot = this->yRot;
    this->yRot = (float)((double)this->yRot - (double)xo * 0.15);
    this->xRot = (float)((double)this->xRot - (double)yo * 0.15);
    if (this->xRot < -90.0f) {
        this->xRot = -90.0f;
    }
    if (this->xRot > 90.0f) {
        this->xRot = 90.0f;
    }

    this->xRotI = this->xRot - xRot;
    this->yRotI = this->yRot - yRot;
}

void Entity::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    xRotI = 0.0f;
    yRotI = 0.0f;
}

bool Entity::isFree(float x, float y, float z) {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return false;
    }
    shared_ptr<AABB> aabb = bb->cloneMove(x, y, z);
    vector<shared_ptr<AABB>> cubes = level->getCubes(aabb);
    return cubes.size() > 0 ? false : !level->containsAnyLiquid(aabb);
}

void Entity::move(float xa, float ya, float za) {
    float xaOrg = xa;
    float yaOrg = ya;
    float zaOrg = za;

    shared_ptr<AABB> expanded = this->bb->expand(xa, ya, za);
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }
    vector<shared_ptr<AABB>> aABBs = level->getCubes(expanded); 
    
    for (int32_t i = 0; i < aABBs.size(); i++) {
        ya = aABBs[i]->clipYCollide(this->bb, ya);
    }

    this->bb->move(0.0f, ya, 0.0f);

    for (int32_t i = 0; i < aABBs.size(); i++) {
        xa = aABBs[i]->clipXCollide(this->bb, xa);
    }

    this->bb->move(xa, 0.0f, 0.0f);

    for (int32_t i = 0; i < aABBs.size(); i++) {
        za = aABBs[i]->clipZCollide(this->bb, za);
    }

    this->bb->move(0.0f, 0.0f, za);
    horizontalCollision = xaOrg != x || zaOrg != z;
    this->onGround = yaOrg != ya && yaOrg < 0.0f;

    if (xaOrg != xa) {
        this->xd = 0.0f;
    }
    if (yaOrg != ya) {
        this->yd = 0.0f;
    }
    if (zaOrg != za) {
        this->zd = 0.0f;
    }

    this->x = (this->bb->minX + this->bb->maxX) / 2.0f;
    this->y = this->bb->minY + this->heightOffset;
    this->z = (this->bb->minZ + this->bb->maxZ) / 2.0f;
}

bool Entity::isInWater() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return false;
    }
    shared_ptr<AABB> newaabb = bb->grow(0.0F, -0.4F, 0.0F);
    return level->containsLiquid(newaabb, 1);
}

bool Entity::isInLava() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return false;
    }
    return level->containsLiquid(bb, 2);
}

void Entity::moveRelative(float xa, float za, float speed) {
    float dist = (float)sqrt((double)(xa * xa + za * za));
    if (dist >= 0.01f) {
        if (dist < 1.0f) {
            dist = 1.0f;
        }

        dist = speed / dist;
        xa *= dist;
        za *= dist;
        float sinv = (float)sin(this->yRot * M_PI / 180.0);
        float cosv = (float)cos(this->yRot * M_PI / 180.0);
        this->xd += xa * cosv - za * sinv;
        this->zd += za * cosv + xa * sinv;
    }
}

bool Entity::isLit() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return false;
    }
    int32_t xTile = (int32_t)this->x;
    int32_t yTile = (int32_t)this->y;
    int32_t zTile = (int32_t)this->z;
    return level->isLit(xTile, yTile, zTile);
}

float Entity::getBrightness() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return 0.0f;
    }
    int32_t x = this->x;
    int32_t y = (int32_t)(this->y + heightOffset / 2.0F);
    int32_t z = this->z;
    return level->getBrightness(x, y, z);
}

void Entity::render(shared_ptr<Textures>& textures, float a) {}

void Entity::setLevel(shared_ptr<Level>& level) {
    this->level = level;
}