#pragma once

#include <stdint.h>
#include <memory>

using namespace std;

class AABB {
public:
    static const int64_t serialVersionUID = 0;
private:
    float epsilon = 0.0f;
public:
    float minX;
    float minY; 
    float minZ; 
    float maxX;
    float maxY;
    float maxZ;

    AABB();

    AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

    shared_ptr<AABB> expand(float x, float y, float z);
    shared_ptr<AABB> grow(float x, float y, float z);
    shared_ptr<AABB> cloneMove(float x, float y, float z);
    float clipXCollide(shared_ptr<AABB>& otherBoundingBox, float x);
    float clipYCollide(shared_ptr<AABB>& otherBoundingBox, float y);
    float clipZCollide(shared_ptr<AABB>& otherBoundingBox, float z);
    bool intersects(shared_ptr<AABB>& otherBoundingBox);
    void move(float x, float y, float z);
};
