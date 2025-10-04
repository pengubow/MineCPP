#include "Entity.h"
#include "Util.h"

Entity::Entity(shared_ptr<Level>& level) {
    this->level = level;
    this->resetPos();
}

void Entity::resetPos() {
    float x = Util::nextFloat() * this->level->width;
    float y = this->level->depth + 10.0f;
    float z = Util::nextFloat() * this->level->height;
    this->setPos(x, y, z);
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
    this->bb = AABB(x - w, y - h, z - w, x + w, y + h, z + w);
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

void Entity::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
}

void Entity::move(float xa, float ya, float za) {
    float xaOrg = xa;
    float yaOrg = ya;
    float zaOrg = za;

    vector<shared_ptr<AABB>> aABBs = this->level->getCubes(this->bb.expand(xa, ya, za)); 

    
    for (int32_t i = 0; i < aABBs.size(); i++) {
        ya = aABBs[i]->clipYCollide(this->bb, ya);
    }

    this->bb.move(0.0f, ya, 0.0f);

    for (int32_t i = 0; i < aABBs.size(); i++) {
        xa = aABBs[i]->clipXCollide(this->bb, xa);
    }

    this->bb.move(xa, 0.0f, 0.0f);

    for (int32_t i = 0; i < aABBs.size(); i++) {
        za = aABBs[i]->clipZCollide(this->bb, za);
    }

    this->bb.move(0.0f, 0.0f, za);

    bool bl = this->onGround = (yaOrg != ya && yaOrg < 0.0f);

    if (xaOrg != xa) {
        this->xd = 0.0f;
    }
    if (yaOrg != ya) {
        this->yd = 0.0f;
    }
    if (zaOrg != za) {
        this->zd = 0.0f;
    }

    this->x = (this->bb.minX + this->bb.maxX) / 2.0f;
    this->y = this->bb.minY + this->heightOffset;
    this->z = (this->bb.minZ + this->bb.maxZ) / 2.0f;
}

void Entity::moveRelative(float xa, float za, float speed) {
    float dist = xa * xa + za * za;
    if (dist < 0.01f) {
        return;
    }
    dist = speed / (float)sqrt(dist);
    float sinv = (float)sin(this->yRot * M_PI / 180.0);
    float cosv = (float)cos(this->yRot * M_PI / 180.0);
    xa *= dist;
    za *= dist;
    this->xd += xa * cosv - za * sinv;
    this->zd += za * cosv + xa * sinv;
}

bool Entity::isLit() {
    int32_t xTile = (int32_t)this->x;
    int32_t yTile = (int32_t)this->y;
    int32_t zTile = (int32_t)this->z;
    return this->level->isLit(xTile, yTile, zTile);
}

void Entity::render(float a) {}