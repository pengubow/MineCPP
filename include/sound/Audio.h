#pragma once

#include <vector>
#include "stdint.h"

using namespace std;

class Audio {
public:
    virtual bool play(vector<int32_t>& var1, vector<int32_t>& var2, int32_t var3);
};