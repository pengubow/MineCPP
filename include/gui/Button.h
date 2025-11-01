#pragma once

#include <stdint.h>
#include <string>
#include "gui/Gui.h"

using namespace std;

class Button : public Gui {
public:
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    string msg;
    int32_t id;
    bool enabled;
    bool visible;

    Button(int32_t id, int32_t x, int32_t y, string msg);
protected:
    Button(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, string msg);
};

