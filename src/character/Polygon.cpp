#include "character/Polygon.h"

Polygon::Polygon(vector<Vertex>& vertices) 
    : vertices(vertices) {}

Polygon::Polygon(vector<Vertex>& vertices, int minU, int minV, int maxU, int maxV) {
    this->vertices.push_back(vertices.at(0).remap(maxU, minV));
    this->vertices.push_back(vertices.at(1).remap(minU, minV));
    this->vertices.push_back(vertices.at(2).remap(minU, maxV));
    this->vertices.push_back(vertices.at(3).remap(maxU, maxV));
}

void Polygon::render() {
    for (int32_t i = 3; i >= 0; --i) {
        Vertex v = this->vertices[i];
        glTexCoord2f(v.u / 63.999f, v.v / 31.999f);
        glVertex3f(v.pos->x, v.pos->y, v.pos->z);
    }
}