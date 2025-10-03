#pragma once

class AABB {
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

    AABB expand(float x, float y, float z);
    AABB grow(float x, float y, float z);
    float clipXCollide(AABB& otherBoundingBox, float x);
    float clipYCollide(AABB& otherBoundingBox, float y);
    float clipZCollide(AABB& otherBoundingBox, float z);
    bool intersects(AABB& otherBoundingBox);
    void move(float x, float y, float z);
};
