#pragma once

#include <cstdint>
#include <cmath>
#include <array>
#include "GL_compat.h"
#include "phys/AABB.h"

using namespace std;

class Frustum {
public:
    static array<array<float,4>,6> m_Frustum;
private:
    static Frustum frustum;
    static array<float,16> proj;
    static array<float,16> modl;
    static array<float,16> clip;

    Frustum();

    static void normalizePlane(array<array<float,4>,6>& frustumArr, int side);
public:
    static Frustum& getFrustum();

    bool cubeInFrustum(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    bool isVisible(AABB& aabb);
};