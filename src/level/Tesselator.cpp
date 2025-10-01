#include <GL/gl.h>
#include "level/Tesselator.h"

std::shared_ptr<Tesselator> Tesselator::instance = std::make_shared<Tesselator>();

Tesselator::Tesselator() 
    : buffer(MAX_FLOATS), array(MAX_FLOATS), 
    vertices(0), u(0.0f), v(0.0f), r(0.0f), g(0.0f), b(0.0f),
    hasColor(false), hasTexture(false) {}

void Tesselator::flush() {
    buffer.assign(array.begin(), array.begin() + p);
    if (this->hasTexture && this->hasColor) {
        glInterleavedArrays(GL_T2F_C3F_V3F, 0, this->buffer.data());
    }
    else if (this->hasTexture) {
        glInterleavedArrays(GL_T2F_V3F, 0, this->buffer.data());
    }
    else if (this->hasColor) {
        glInterleavedArrays(GL_C3F_V3F, 0, this->buffer.data());
    }
    else {
        glInterleavedArrays(GL_V3F, 0, this->buffer.data());
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    if (hasTexture) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (hasColor) glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_QUADS, 0, vertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (hasTexture) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (hasColor) glDisableClientState(GL_COLOR_ARRAY);

    clear();
}

void Tesselator::clear() {
    vertices = 0;
    p = 0;
}

void Tesselator::init() {
    clear();
    hasColor = false;
    hasTexture = false;
}

void Tesselator::tex(float uu, float vv) {
    if (!this->hasTexture) {
        this->len += 2;
    }
    hasTexture = true;
    u = uu;
    v = vv;
}

void Tesselator::color(float rr, float gg, float bb) {
    if (!this->hasColor) {
        this->len += 3;
    }
    hasColor = true;
    r = rr;
    g = gg;
    b = bb;
}

void Tesselator::vertexUV(float x, float y, float z, float u, float v) {
    this->tex(u, v);
    this->vertex(x, y, z);
}

void Tesselator::vertex(float x, float y, float z) {
    if (hasTexture) {
        this->array.at(this->p++) = u;
        this->array.at(this->p++) = v;
    }

    if (hasColor) {
        this->array.at(this->p++) = r;
        this->array.at(this->p++) = g;
        this->array.at(this->p++) = b;
    }

    this->array.at(this->p++) = x;
    this->array.at(this->p++) = y;
    this->array.at(this->p++) = z;

    ++vertices;
    if (this->vertices % 4 == 0 && this->p >= MAX_FLOATS - this->len * 4) {
        flush();
    }
}