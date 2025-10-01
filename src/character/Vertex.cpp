#include "character/Vertex.h"
#include "character/Vec3.h"

Vertex::Vertex(float x, float y, float z, float u, float v)
    : pos(std::make_shared<Vec3>(x, y, z)), u(u), v(v) {}

Vertex Vertex::remap(float u, float v) {
    return Vertex(*this, u, v);
}

Vertex::Vertex(Vertex& vertex, float u, float v)
    : pos(vertex.pos), u(u), v(v) {}

Vertex::Vertex(std::shared_ptr<Vec3>& pos, float u, float v)
    : pos(pos), u(u), v(v) {}


