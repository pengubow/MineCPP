#pragma once

#include "gui/Button.h"

class KeyBindingButton : public Button {
public:
    KeyBindingButton(int32_t id, int32_t x, int32_t y, string msg);
};