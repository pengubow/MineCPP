#include <iostream>
#include "GL_compat.h"
#include "gui/Font.h"
#include "stb_image.h"
#include "renderer/Tesselator.h"

Font::Font(string name, shared_ptr<Textures>& textures) {
    int32_t w;
    int32_t h;
    int32_t channels;
    uint8_t* img = stbi_load(name.c_str(), &w, &h, &channels, 4);
    if (!img) {
        throw runtime_error("Failed to load font image: " + name);
    }
    for (int32_t i = 0; i < 128; i++) {
        int32_t xt = i % 16;
        int32_t yt = i / 16;
        int32_t x = 0;
        bool emptyColumn = false;
        for (x = 0; x < 8 && !emptyColumn; x++) {
            int32_t xPixel = xt * 8 + x;
            emptyColumn = true;
            for (int32_t y = 0; y < 8 && emptyColumn; y++) {
                int32_t yPixel = yt * 8 + y;
                int32_t pixel = img[((yPixel * w + xPixel) * 4) + 2];
                if (pixel > 128) {
                    emptyColumn = false;
                }
            }
        }
        if (i == 32) {
            x = 4;
        }
        this->charWidths[i] = x;
    }
    this->fontTexture = textures->loadTexture(name, GL_NEAREST);
    stbi_image_free(img);
}

void Font::drawShadow(string str, int32_t x, int32_t y, int32_t color) {
    draw(str, x + 1, y + 1, color, true);
    draw(str, x, y, color);
}

void Font::draw(string str, int32_t x, int32_t y, int32_t color) {
    draw(str, x, y, color, false);
}

void Font::draw(string str, int32_t x, int32_t y, int32_t color, bool darken) {
    if (darken) {
        color = (color & 0xFCFCFC) >> 2;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    shared_ptr<Tesselator> t = Tesselator::instance;
    t->begin();
    t->color(color);
    int32_t xo = 0;
    for (int32_t i = 0; i < str.length(); i++) {
        if (str[i] == '&') {
            int32_t cc = string("0123456789abcdef").find(str[i + 1]);
            if (cc != string::npos) {
                int32_t br = (cc & 8) * 8;
                int32_t b = (cc & 1) * 191 + br;
                int32_t g = ((cc & 2) >> 1) * 191 + br;
                int32_t r = ((cc & 4) >> 2) * 191 + br;
                color = r << 16 | g << 8 | b;
                i += 2;
                if (darken) {
                    color = (color & 0xFCFCFC) >> 2;
                }
                t->color(color);
            }
            
        }
        int32_t ix = str[i] % 16 * 8;
        int32_t iy = str[i] / 16 * 8;
        float var13 = 7.99f;
        t->vertexUV((float)(x + xo), (float)y + var13, 0.0f, (float)ix / 128.0f, (float)(iy + var13) / 128.0f);
        t->vertexUV((float)(x + xo) + var13, (float)y + var13, 0.0f, ((float)ix + var13) / 128.0f, ((float)iy + var13) / 128.0f);
        t->vertexUV((float)(x + xo) + var13, y, 0.0f, ((float)ix + var13) / 128.0f, (float)iy / 128.0f);
        t->vertexUV((float)(x + xo), y, 0.0f, (float)ix / 128.0f, (float)iy / 128.0f);
        xo += this->charWidths[str[i]];
    }
    t->end();
    glDisable(GL_TEXTURE_2D);
}

int32_t Font::width(string str) {
    int32_t len = 0;
    for (int32_t i = 0; i < str.size(); i++) {
        if (str[i] == '&') {
            i++;
        }
        else {
            len += this->charWidths[str[i]];
        }
    }
    return len;
}