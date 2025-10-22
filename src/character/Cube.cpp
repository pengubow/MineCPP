#include "character/Cube.h"

Cube::Cube(int32_t xTexOffs, int32_t yTexOffs) 
    : xTexOffs(xTexOffs), yTexOffs(yTexOffs),
    xRot(0.0f), yRot(0.0f), zRot(0.0f), 
    x(0.0f), y(0.0f), z(0.0f) {}

void Cube::setTexOffs(int32_t xTexOffs, int32_t yTexOffs) {
    this->xTexOffs = xTexOffs;
    this->yTexOffs = yTexOffs;
}

void Cube::addBox(float minX, float minY, float minZ, int32_t w, int32_t h, int32_t d) {
    float maxX = minX + w;
    float maxY = minY + h;
    float maxZ = minZ + d;

    Vertex minU = Vertex(minX, minY, minZ, 0.0f, 0.0f);
    Vertex maxU = Vertex(maxX, minY, minZ, 0.0f, 8.0f);
    Vertex u2 = Vertex(maxX, maxY, minZ, 8.0f, 8.0f);
    Vertex u3 = Vertex(minX, maxY, minZ, 8.0f, 0.0f);
    
    Vertex l0 = Vertex(minX, minY, maxZ, 0.0f, 0.0f);
    Vertex l1 = Vertex(maxX, minY, maxZ, 0.0f, 8.0f);
    Vertex l2 = Vertex(maxX, maxY, maxZ, 8.0f, 8.0f);
    Vertex l3 = Vertex(minX, maxY, maxZ, 8.0f, 0.0f);

    this->vertices.push_back(minU);
    this->vertices.push_back(maxU);
    this->vertices.push_back(u2);
    this->vertices.push_back(u3);
    this->vertices.push_back(l0);
    this->vertices.push_back(l1);
    this->vertices.push_back(l2);
    this->vertices.push_back(l3);

    auto hi = vector<Vertex>{ l1, maxU, u2, l2 };
    auto hi1 = vector<Vertex>{ minU, l0, l3, u3 };
    auto hi2 = vector<Vertex>{ l1, l0, minU, maxU };
    auto hi3 = vector<Vertex>{ u2, u3, l3, l2 };
    auto hi4 = vector<Vertex>{ maxU, minU, u3, u2 };
    auto hi5 = vector<Vertex>{ l0, l1, l2, l3 };
    this->polygons.push_back(Quad(hi, this->xTexOffs + d + w, this->yTexOffs + d, this->xTexOffs + d + w + d, this->yTexOffs + d + h));
    this->polygons.push_back(Quad(hi1, this->xTexOffs + 0, this->yTexOffs + d, this->xTexOffs + d, this->yTexOffs + d + h));
    this->polygons.push_back(Quad(hi2, this->xTexOffs + d, this->yTexOffs + 0, this->xTexOffs + d + w, this->yTexOffs + d));
    this->polygons.push_back(Quad(hi3, this->xTexOffs + d + w, this->yTexOffs + 0, this->xTexOffs + d + w + w, this->yTexOffs + d));
    this->polygons.push_back(Quad(hi4, this->xTexOffs + d, this->yTexOffs + d, this->xTexOffs + d + w, this->yTexOffs + d + h));
    this->polygons.push_back(Quad(hi5, this->xTexOffs + d + w + d, this->yTexOffs + d, this->xTexOffs + d + w + d + w, this->yTexOffs + d + h));
}

void Cube::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Cube::render() {
    if (!this->compiled) {
        this->list = glGenLists(1);
        glNewList(this->list, GL_COMPILE);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < this->polygons.size(); i++) {
            for (int32_t i2 = 3; i2 >= 0; --i2) {
                Vertex v = this->polygons[i].vertices[i2];
                glTexCoord2f(v.u / 64.0f, v.v / 32.0f);
                glVertex3f(v.pos->x, v.pos->y, v.pos->z);
            }
        }
        glEnd();
        glEndList();
        this->compiled = true;
    }
    float c = 57.29578f;
    glPushMatrix();
    glTranslatef(this->x, this->y, this->z);
    glRotatef(this->zRot * c, 0.0f, 0.0f, 1.0f);
    glRotatef(this->yRot * c, 0.0f, 1.0f, 0.0f);
    glRotatef(this->xRot * c, 1.0f, 0.0f, 0.0f);
    glCallList(this->list);
    glPopMatrix();
}