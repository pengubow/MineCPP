#pragma once

#include <vector>
#include <memory>
#include "renderer/Textures.h"

class Font {
    vector<int32_t> charWidths = vector<int32_t>(256);
    int32_t fontTexture = 0;
public:
    Font(string name, shared_ptr<Textures>& textures);

    void drawShadow(string str, int32_t x, int32_t y, int32_t color);
    void draw(string str, int32_t x, int32_t y, int32_t color);
private:
    void draw(string str, int32_t x, int32_t y, int32_t color, bool darken);
public:
    int32_t width(string str);
    static string removeColorCodes(string var0);
};