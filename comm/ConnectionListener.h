#pragma once

#include <stdint.h>
#include <exception>
#include <vector>

using namespace std;

class ConnectionListener {
public:
    virtual void handleException(const exception& exception) = 0;
    virtual void command(uint8_t var1, int32_t var2, vector<uint8_t>& var3) = 0;
};