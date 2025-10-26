#include <cmath>
#include "character/Vec3.h"

Vec3::Vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3 Vec3::subtract(shared_ptr<Vec3>& vec3) {
    return Vec3(x - vec3->x, y - vec3->y, z - vec3->z);
}

Vec3 Vec3::normalize() {
    float var1 = (float)sqrt((double)(x * x + y * y + z * z));
    return Vec3(x / var1, y / var1, z / var1);
}