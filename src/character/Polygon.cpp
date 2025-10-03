#include "character/Polygon.h"

Polygon::Polygon(vector<Vertex>& vertices) 
    : vertices(vertices), vertexCount(vertices.size()) {}

Polygon::Polygon(vector<Vertex>& vertices, int u0, int v0, int u1, int v1) 
    : vertexCount(vertices.size()) {
    this->vertices.push_back(vertices.at(0).remap(u1, v0));
    this->vertices.push_back(vertices.at(1).remap(u0, v0));
    this->vertices.push_back(vertices.at(2).remap(u0, v1));
    this->vertices.push_back(vertices.at(3).remap(u1, v1));
}

void Polygon::render() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int32_t i = 3; i >= 0; --i) {
        Vertex v = this->vertices[i];
        glTexCoord2f(v.u / 63.999f, v.v / 31.999f);
        glVertex3f(v.pos->x, v.pos->y, v.pos->z);
    }
}