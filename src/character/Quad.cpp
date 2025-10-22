#include "character/Quad.h"

Quad::Quad(vector<Vertex>& vertices) 
    : vertices(vertices) {}

Quad::Quad(vector<Vertex>& vertices, int minU, int minV, int maxU, int maxV) {
    this->vertices.push_back(vertices.at(0).remap(maxU, minV));
    this->vertices.push_back(vertices.at(1).remap(minU, minV));
    this->vertices.push_back(vertices.at(2).remap(minU, maxV));
    this->vertices.push_back(vertices.at(3).remap(maxU, maxV));
}