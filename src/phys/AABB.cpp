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

AABB AABB::cloneMove(float x, float y, float z) {
    return AABB(minX + x, minY + y, minZ + z, maxX + x, maxY + y, maxZ + z);
}

float AABB::clipXCollide(AABB& otherBoundingBox, float x) {
    if (otherBoundingBox.maxY > minY && otherBoundingBox.minY < maxY) {
        if (otherBoundingBox.maxZ > minZ && otherBoundingBox.minZ < maxZ) {
            float max;
            if (x > 0.0f && otherBoundingBox.maxX <= minX) {
                max = minX - otherBoundingBox.maxX - epsilon;
                if (max < x) {
                    x = max;
                }
            }

            if (x < 0.0f && otherBoundingBox.minX >= maxX) {
                max = maxX - otherBoundingBox.minX + epsilon;
                if (max > x) {
                    x = max;
                }
            }

            return x;
        } else {
            return x;
        }
    } else {
        return x;
    }
}

float AABB::clipYCollide(AABB& otherBoundingBox, float y) {
    if (otherBoundingBox.maxX > minX && otherBoundingBox.minX < maxX) {
        if (otherBoundingBox.maxZ > minZ && otherBoundingBox.minZ < maxZ) {
            float max;
            if (y > 0.0f && otherBoundingBox.maxY <= minY) {
                max = minY - otherBoundingBox.maxY - epsilon;
                if (max < y) {
                    y = max;
                }
            }

            if (y < 0.0f && otherBoundingBox.minY >= maxY) {
                max = maxY - otherBoundingBox.minY + epsilon;
                if (max > y) {
                    y = max;
                }
            }

            return y;
        } else {
            return y;
        }
    } else {
        return y;
    }
}

float AABB::clipZCollide(AABB& otherBoundingBox, float x) {
    if (otherBoundingBox.maxX > minX && otherBoundingBox.minX < maxX) {
        if (otherBoundingBox.maxY > minY && otherBoundingBox.minY < maxY) {
            float max;
            if (x > 0.0f && otherBoundingBox.maxZ <= minZ) {
                max = minZ - otherBoundingBox.maxZ - epsilon;
                if(max < x) {
                    x = max;
                }
            }

            if (x < 0.0f && otherBoundingBox.minZ >= maxZ) {
                max = maxZ - otherBoundingBox.minZ + epsilon;
                if(max > x) {
                    x = max;
                }
            }

            return x;
        } else {
            return x;
        }
    } else {
        return x;
    }
}

bool AABB::intersects(AABB& otherBoundingBox) {
    return otherBoundingBox.maxX > minX && otherBoundingBox.minX < maxX ? (otherBoundingBox.maxY > minY && otherBoundingBox.minY < maxY ? otherBoundingBox.maxZ > minZ && otherBoundingBox.minZ < maxZ : false) : false;
}

void AABB::move(float x, float y, float z) {
    this->minX += x;
    this->minY += y;
    this->minZ += z;
    this->maxX += x;
    this->maxY += y;
    this->maxZ += z;
}