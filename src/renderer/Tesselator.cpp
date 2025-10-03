#include <GL/gl.h>
#include "renderer/Tesselator.h"

shared_ptr<Tesselator> Tesselator::instance = make_shared<Tesselator>();

Tesselator::Tesselator() 
    : buffer(MAX_FLOATS), array(MAX_FLOATS), 
    vertices(0), u(0.0f), v(0.0f), r(0.0f), g(0.0f), b(0.0f),
    hasColor(false), hasTexture(false) {}

void Tesselator::flush() {
    if (vertices > 0) {
        buffer.assign(array.begin(), array.begin() + p);
        if (hasTexture && hasColor) {
            glInterleavedArrays(GL_T2F_C3F_V3F, 0, buffer.data());
        }
        else if (hasTexture) {
            glInterleavedArrays(GL_T2F_V3F, 0, buffer.data());
        }
        else if (hasColor) {
            glInterleavedArrays(GL_C3F_V3F, 0, buffer.data());
        }
        else {
            glInterleavedArrays(GL_V3F, 0, buffer.data());
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        if (hasTexture) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        if (hasColor) {
            glEnableClientState(GL_COLOR_ARRAY);
        }
        glDrawArrays(GL_QUADS, 0, vertices);
        glDisableClientState(GL_VERTEX_ARRAY);
        if (hasTexture) {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        if (hasColor) {
            glDisableClientState(GL_COLOR_ARRAY);
        }
    }

    clear();
}

void Tesselator::clear() {
    vertices = 0;
    p = 0;
    noColor = false;
}

void Tesselator::init() {
    clear();
    hasColor = false;
    hasTexture = false;
}

void Tesselator::tex(float u, float v) {
    if (!hasTexture) {
        len += 2;
    }
    hasTexture = true;
    this->u = u;
    this->v = v;
}

void Tesselator::color(float r, float g, float b) {
    if (noColor) {
        return;
    }
    if (!hasColor) {
        len += 3;
    }
    hasColor = true;
    this->r = r;
    this->g = g;
    this->b = b;
}

void Tesselator::vertexUV(float x, float y, float z, float u, float v) {
    tex(u, v);
    vertex(x, y, z);
}

void Tesselator::vertex(float x, float y, float z) {
    if (hasTexture) {
        array.at(p++) = u;
        array.at(p++) = v;
    }

    if (hasColor) {
        array.at(p++) = r;
        array.at(p++) = g;
        array.at(p++) = b;
    }

    array.at(p++) = x;
    array.at(p++) = y;
    array.at(p++) = z;

    ++vertices;
    if (vertices % 4 == 0 && p >= MAX_FLOATS - len * 4) {
        flush();
    }
}

void Tesselator::color(int32_t c) {
    float r = (float)(c >> 16 & 0xFF) / 255.0f;
    float g = (float)(c >> 8 & 0xFF) / 255.0f;
    float b = (float)(c & 0xFF) / 255.0f;
    color(r, g, b);
}

void Tesselator::setNoColor() {
    noColor = true;
}