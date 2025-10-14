#pragma once

#include <stdint.h>

using namespace std;

class MovementInput {
public:
    float moveStrafe = 0.0f;
    float moveForward = 0.0f;
    bool jump = false;
    bool jumpHeld = false;

    void updatePlayerMoveState();
    void releaseAllKeys();
    void setKey(int32_t key, bool state);
};
