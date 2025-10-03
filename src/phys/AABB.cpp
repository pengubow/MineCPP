#include "phys/AABB.h"

AABB::AABB() 
    : minX(0), minY(0), minZ(0), 
    maxX(0), maxY(0), maxZ(0) {}

AABB::AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) 
    : minX(minX), minY(minY), minZ(minZ), 
    maxX(maxX), maxY(maxY), maxZ(maxZ) {}

AABB AABB::expand(float x, float y, float z) {
    float minX = this->minX;
    float minY = this->minY;
    float minZ = this->minZ;
    float maxX = this->maxX;
    float maxY = this->maxY;
    float maxZ = this->maxZ;

    if (x < 0.0f) {
        minX += x;
    }
    if (x > 0.0f) {
        maxX += x;
    }
    if (y < 0.0f) {
        minY += y;
    }
    if (y > 0.0f) {
        maxY += y;
    }
    if (z < 0.0f) {
        minZ += z;
    }
    if (z > 0.0f) {
        maxZ += z;
    }
    return AABB(minX, minY, minZ, maxX, maxY, maxZ);
}

AABB AABB::grow(float x, float y, float z) {
    float minX = this->minX - x;
    float minY = this->minY - y;
    float minZ = this->minZ - z;
    float maxX = this->maxX + x;
    float maxY = this->maxY + y;
    float maxZ = this->maxZ + z;
    return AABB(minX, minY, minZ, maxX, maxY, maxZ);
}

float AABB::clipXCollide(AABB& otherBoundingBox, float x) {
    float max;
    if (otherBoundingBox.maxY <= this->minY || otherBoundingBox.minY >= this->maxY) {
        return x;
    }
    if (otherBoundingBox.maxZ <= this->minZ || otherBoundingBox.minZ >= this->maxZ) {
        return x;
    }
    if (x > 0.0f && otherBoundingBox.maxX <= this->minX && (max = this->minX - otherBoundingBox.maxX - this->epsilon) < x) {
        x = max;
    }
    if (x < 0.0f && otherBoundingBox.minX >= this->maxX && (max = this->maxX - otherBoundingBox.minX + this->epsilon) > x) {
        x = max;
    }
    return x;
}

float AABB::clipYCollide(AABB& otherBoundingBox, float y) {
    float max;
    if (otherBoundingBox.maxX <= this->minX || otherBoundingBox.minX >= this->maxX) {
        return y;
    }
    if (otherBoundingBox.maxZ <= this->minZ || otherBoundingBox.minZ >= this->maxZ) {
        return y;
    }
    if (y > 0.0f && otherBoundingBox.maxY <= this->minY && (max = this->minY - otherBoundingBox.maxY - this->epsilon) < y) {
        y = max;
    }
    if (y < 0.0f && otherBoundingBox.minY >= this->maxY && (max = this->maxY - otherBoundingBox.minY + this->epsilon) > y) {
        y = max;
    }
    return y;
}

float AABB::clipZCollide(AABB& otherBoundingBox, float z) {
    float max;
    if (otherBoundingBox.maxX <= this->minX || otherBoundingBox.minX >= this->maxX) {
        return z;
    }
    if (otherBoundingBox.maxY <= this->minY || otherBoundingBox.minY >= this->maxY) {
        return z;
    }
    if (z > 0.0f && otherBoundingBox.maxZ <= this->minZ && (max = this->minZ - otherBoundingBox.maxZ - this->epsilon) < z) {
        z = max;
    }
    if (z < 0.0f && otherBoundingBox.minZ >= this->maxZ && (max = this->maxZ - otherBoundingBox.minZ + this->epsilon) > z) {
        z = max;
    }
    return z;
}

bool AABB::intersects(AABB& otherBoundingBox) {
    if (otherBoundingBox.maxX <= this->minX || otherBoundingBox.minX >= this->maxX) {
        return false;
    }
    if (otherBoundingBox.maxY <= this->minY || otherBoundingBox.minY >= this->maxY) {
        return false;
    }
    return !(otherBoundingBox.maxZ <= this->minZ) && !(otherBoundingBox.minZ >= this->maxZ);
}

void AABB::move(float x, float y, float z) {
    this->minX += x;
    this->minY += y;
    this->minZ += z;
    this->maxX += x;
    this->maxY += y;
    this->maxZ += z;
}