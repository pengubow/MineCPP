#pragma once

#include <stdint.h>
#include <string>

using namespace std;

class Button {
public:
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    string msg;
    int32_t id;
    bool enabled = true;
    bool visible = true;

    Button(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, string msg);
};

