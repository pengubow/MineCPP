#include <GL/gl.h>
#include "character/Polygon.h"

class Cube {
private:
    std::vector<Vertex> vertices;
    std::vector<Polygon> polygons;
    int32_t xTexOffs;
    int32_t yTexOffs;
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float xRot;
    float yRot;
    float zRot;
private:
    bool compiled = false;
    int32_t list = 0;
public:
    Cube(int32_t xTexOffs = 0, int32_t yTexOffs = 0);

    void setTexOffs(int32_t xTexOffs, int32_t yTexOffs);
    void addBox(float minX, float minY, float minZ, int32_t w, int32_t h, int32_t d);
    void setPos(float x, float y, float z);
    void render();
    void compile();
};