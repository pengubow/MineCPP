#pragma once

#include <memory>

using namespace std;

class Vec3 {
public:
    float x;
    float y;
    float z;

    Vec3(float x, float y, float z);

    Vec3 subtract(shared_ptr<Vec3>& vec3);
    Vec3 normalize();
};