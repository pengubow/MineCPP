#include <cmath>
#include "particle/Particle.h"
#include "Util/Util.h"

Particle::Particle(shared_ptr<Level>& level, float x, float y, float z, float xa, float ya, float za, int32_t tex) 
    : Entity(level), tex(tex) {
    this->setSize(0.2f, 0.2f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x, y, z);
    this->xd = xa + (float)(Util::nextFloat() * 2.0 - 1.0) * 0.4f;
    this->yd = ya + (float)(Util::nextFloat() * 2.0 - 1.0) * 0.4f;
    this->zd = za + (float)(Util::nextFloat() * 2.0 - 1.0) * 0.4f;
    float speed = (float)(Util::nextFloat() + Util::nextFloat() + 1.0) * 0.15;
    float dd = (float)sqrt(this->xd * this->xd + this->yd * this->yd + this->zd * this->zd);
    this->xd = this->xd / dd * speed * 0.4f;
    this->yd = this->yd / dd * speed * 0.4f + 0.1f;
    this->zd = this->zd / dd * speed * 0.4f;
    this->uo = Util::nextFloat() * 3.0f;
    this->vo = Util::nextFloat() * 3.0f;
    this->size = (float)(Util::nextFloat() * 0.5 + 0.5);
    this->lifetime = (int32_t)(4.0 / (Util::nextFloat() * 0.9 + 0.1));
    this->age = 0;

    // needed, trust ok maybe not now havent tested yet
    /*this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;*/
}

void Particle::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    if (this->age++ >= this->lifetime) {
        this->remove();
    }
    this->yd = (float)((double)this->yd - 0.04);
    this->move(this->xd, this->yd, this->zd);
    this->xd *= 0.98f;
    this->yd *= 0.98f;
    this->zd *= 0.98f;
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
    }
}

void Particle::render(shared_ptr<Tesselator>& t, float a, float xa, float ya, float za, float xa2, float za2) {
    float minU = ((float)(this->tex % 16) + this->uo / 4.0f) / 16.0f;
    float maxU = minU + 0.999f / 64.0f;
    float minV = ((float)(this->tex / 16) + this->vo / 4.0f) / 16.0f;
    float maxV = minV + 0.999f / 64.0f;
    float r = 0.1f * this->size;
    float x = this->xo + (this->x - this->xo) * a;
    float y = this->yo + (this->y - this->yo) * a;
    float z = this->zo + (this->z - this->zo) * a;
    t->vertexUV(x - xa * r - xa2 * r, y - ya * r, z - za * r - za2 * r, minU, maxV);
    t->vertexUV(x - xa * r + xa2 * r, y + ya * r, z - za * r + za2 * r, minU, minV);
    t->vertexUV(x + xa * r + xa2 * r, y + ya * r, z + za * r + za2 * r, maxU, minV);
    t->vertexUV(x + xa * r - xa2 * r, y - ya * r, z + za * r - za2 * r, maxU, maxV);
}