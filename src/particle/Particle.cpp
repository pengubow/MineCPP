#include <cmath>
#include "level/tile/Tile.h"
#include "particle/Particle.h"

Particle::Particle(shared_ptr<Level>& level, float x, float y, float z, float xa, float ya, float za, Tile* tile) 
    : Entity(level) {
    tex = tile->tex;
    gravity = tile->particleGravity;
    setSize(0.2f, 0.2f);
    heightOffset = bbHeight / 2.0f;
    setPos(x, y, z);
    xd = xa + (float)(Random::random() * 2.0 - 1.0) * 0.4f;
    yd = ya + (float)(Random::random() * 2.0 - 1.0) * 0.4f;
    zd = za + (float)(Random::random() * 2.0 - 1.0) * 0.4f;
    float speed = (float)(Random::random() + Random::random() + 1.0) * 0.15;
    float dd = (float)sqrt(xd * xd + yd * yd + zd * zd);
    xd = xd / dd * speed * 0.4f;
    yd = yd / dd * speed * 0.4f + 0.1f;
    zd = zd / dd * speed * 0.4f;
    uo = (float)Random::random() * 3.0f;
    vo = (float)Random::random() * 3.0f;
    size = (float)(Random::random() * 0.5 + 0.5);
    lifetime = (int32_t)(4.0 / (Random::random() * 0.9 + 0.1));
    age = 0;
    makeStepSound = false;
}

void Particle::tick() {
    xo = x;
    yo = y;
    zo = z;
    if (age++ >= lifetime) {
        remove();
    }
    yd = (float)((double)yd - 0.04 * gravity);
    move(xd, yd, zd);
    xd *= 0.98f;
    yd *= 0.98f;
    zd *= 0.98f;
    if (onGround) {
        xd *= 0.6f;
        zd *= 0.6f;
    }
}

void Particle::render(shared_ptr<Tesselator>& t, float a, float xa, float ya, float za, float xa2, float za2) {
    float minU = ((float)(tex % 16) + uo / 4.0f) / 16.0f;
    float maxU = minU + 0.999f / 64.0f;
    float minV = ((float)(tex / 16) + vo / 4.0f) / 16.0f;
    float maxV = minV + 0.999f / 64.0f;
    float r = 0.1f * size;
    float x = xo + (this->x - xo) * a;
    float y = yo + (this->y - yo) * a;
    float z = zo + (this->z - zo) * a;
    t->vertexUV(x - xa * r - xa2 * r, y - ya * r, z - za * r - za2 * r, minU, maxV);
    t->vertexUV(x - xa * r + xa2 * r, y + ya * r, z - za * r + za2 * r, minU, minV);
    t->vertexUV(x + xa * r + xa2 * r, y + ya * r, z + za * r + za2 * r, maxU, minV);
    t->vertexUV(x + xa * r - xa2 * r, y - ya * r, z + za * r - za2 * r, maxU, maxV);
}