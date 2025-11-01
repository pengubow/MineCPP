#include "gui/Button.h"

Button::Button(int32_t id, int32_t x, int32_t y, string msg) 
    : Button(id, x, y, 200, 20, msg) {}

Button::Button(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, string msg) 
    : id(id), w(x), h(y), x(w), 
    y(h), msg(msg), enabled(true),
    visible(true) {}