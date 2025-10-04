#include <cstdint>
#include <vector>
#include <GL/gl.h>
#include "character/Vertex.h"

class Polygon {
public:
    vector<Vertex> vertices;
    int32_t vertexCount = 0;

    Polygon(vector<Vertex>& vertices);

    Polygon(vector<Vertex>& vertices, int minU, int minV, int maxU, int maxV);

    void render();
};