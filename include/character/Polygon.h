#include <cstdint>
#include <vector>
#include <GL/gl.h>
#include "character/Vertex.h"

class Polygon {
public:
    std::vector<Vertex> vertices;
    int32_t vertexCount = 0;

    Polygon(std::vector<Vertex>& vertices);

    Polygon(std::vector<Vertex>& vertices, int u0, int v0, int u1, int v1);

    void render();
};