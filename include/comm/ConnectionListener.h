#pragma once

#include <stdint.h>
#include <exception>
#include <vector>

using namespace std;

class ConnectionListener {
public:
    virtual void handleException(const exception& e) = 0;
    virtual void command(uint8_t cmd, int32_t remaining, vector<uint8_t>& in) = 0;
};