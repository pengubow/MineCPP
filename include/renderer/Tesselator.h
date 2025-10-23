#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "GL_compat.h"

using namespace std;

class Tesselator {
    static const int32_t MAX_VERTICES = 4194304;
    static const int32_t MAX_FLOATS = 524288;
    vector<float> buffer;
    vector<float> array;
    int32_t vertices;
    float u;
    float v;
    float r;
    float g;
    float b;
    bool hasColor;
    bool hasTexture;
    int32_t len = 3;
    int32_t p = 0;
public:
    bool noColor = false;
    static shared_ptr<Tesselator> instance;

    Tesselator();

    void end();
    void clear();
    void begin();
    void color(float r, float g, float b);
    void color(int32_t r, int32_t g, int32_t b);
    void vertexUV(float x, float y, float z, float u, float v);
    void vertex(float x, float y, float z);
    void color(int32_t c);
    void setNoColor();
};