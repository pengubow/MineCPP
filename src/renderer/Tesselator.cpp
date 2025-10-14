#include <GL/gl.h>
#include "renderer/Tesselator.h"

shared_ptr<Tesselator> Tesselator::instance = make_shared<Tesselator>();

Tesselator::Tesselator() 
    : buffer(MAX_FLOATS), array(MAX_FLOATS), 
    vertices(0), u(0.0f), v(0.0f), r(0.0f), g(0.0f), b(0.0f),
    hasColor(false), hasTexture(false) {}

void Tesselator::end() {
    if (vertices > 0) {
        buffer.clear();
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
    buffer.clear();
    len = 3;
}

void Tesselator::begin() {
    clear();
    hasColor = false;
    hasTexture = false;
    noColor = false;
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

void Tesselator::color(int32_t r, int32_t g, int32_t b) {
    if(!hasColor) {
        len += 3;
    }
    hasColor = true;
    this->r = (float)(r & 255) / 255.0F;
    this->g = (float)(g & 255) / 255.0F;
    this->b = (float)(b & 255) / 255.0F;
}

void Tesselator::vertexUV(float x, float y, float z, float u, float v) {
    if (!hasTexture) {
        len += 2;
    }

    hasTexture = true;
    this->u = u;
    this->v = v;
    vertex(x, y, z);
}

void Tesselator::vertex(float x, float y, float z) {
    if (hasTexture) {
        array[p++] = u;
        array[p++] = v;
    }

    if (hasColor) {
        array[p++] = r;
        array[p++] = g;
        array[p++] = b;
    }

    array[p++] = x;
    array[p++] = y;
    array[p++] = z;

    ++vertices;
    if (vertices % 4 == 0 && p >= MAX_FLOATS - len * 4) {
        end();
    }
}

void Tesselator::color(int32_t c) {
    int32_t r = c >> 16 & 255;
    int32_t g = c >> 8 & 255;
    c &= 255;
    color(r, g, c);
}

void Tesselator::setNoColor() {
    noColor = true;
}