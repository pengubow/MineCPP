#include "character/Vec3.h"

Vec3::Vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3 Vec3::interpolateTo(Vec3 t, float p) {
    float xt = this->x + (t.x - this->x) * p;
    float yt = this->y + (t.y - this->y) * p;
    float zt = this->z + (t.z - this->z) * p;
    return Vec3(xt, yt, zt);
}

void Vec3::set(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}