#include "gui/Button.h"

Button::Button(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, string msg) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->msg = msg;
}