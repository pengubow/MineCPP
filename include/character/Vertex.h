#pragma once

#include <memory>
#include "character/Vec3.h"

class Vertex {
public:
    std::shared_ptr<Vec3> pos;
    float u;
    float v;

    Vertex(float x, float y, float z, float u, float v);

    Vertex remap(float u, float v);

    Vertex(Vertex& vertex, float u, float v);

    Vertex(std::shared_ptr<Vec3>& pos, float u, float v);
};