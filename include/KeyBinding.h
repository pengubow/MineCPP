#pragma once

#include <string>
#include "stdint.h"

using namespace std;

class KeyBinding {
public:
    string name;
    int32_t key;

    KeyBinding(string name, int32_t key);
};