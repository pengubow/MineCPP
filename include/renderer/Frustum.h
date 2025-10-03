#pragma once

#include <cstdint>
#include <cmath>
#include <array>
#include <GL/gl.h>
#include "phys/AABB.h"

using namespace std;

class Frustum {
public:
    array<array<float,4>,6> m_Frustum;
    static const int RIGHT = 0;
    static const int LEFT = 1;
    static const int BOTTOM = 2;
    static const int TOP = 3;
    static const int BACK = 4;
    static const int FRONT = 5;
    static const int A = 0;
    static const int B = 1;
    static const int C = 2;
    static const int D = 3;
private:
    static Frustum frustum;
    array<float,16> proj;
    array<float,16> modl;
    array<float,16> clip;

    Frustum();

    void normalizePlane(array<array<float,4>,6>& frustumArr, int side);
    void calculateFrustum();
public:
    static Frustum& getFrustum();

    bool pointInFrustum(float x, float y, float z);
    bool sphereInFrustum(float x, float y, float z, float radius);
    bool cubeFullyInFrustum(float maxX, float maxY, float maxZ, float x2, float y2, float z2);
    bool cubeInFrustum(float maxX, float maxY, float maxZ, float x2, float y2, float z2);
    bool isVisible(AABB& aabb);
};