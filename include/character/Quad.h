#pragma once

#include <cstdint>
#include <vector>
#include "GL_compat.h"
#include "character/Vertex.h"

class Quad {
public:
    vector<Vertex> vertices;
    
    Quad(vector<Vertex>& vertices);

    Quad(vector<Vertex>& vertices, int minU, int minV, int maxU, int maxV);
};