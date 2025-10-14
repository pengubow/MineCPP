#pragma once

#include <vector>
#include "player/MovementInput.h"

class MovementInputFromOptions : public MovementInput {
private:
    bool keys[10];
public:
    void setKey(int32_t key, bool state);
    void releaseAllKeys();
    void updatePlayerMoveState();
};