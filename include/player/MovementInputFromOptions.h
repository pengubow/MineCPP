#pragma once

#include <vector>
#include <memory>
#include "player/MovementInput.h"
#include "Options.h"

class MovementInputFromOptions : public MovementInput {
    vector <bool> keys = vector<bool>(10, 0);
    Options* options;
public:
    MovementInputFromOptions(Options* options);

    void setKey(int32_t key, bool state);
    void releaseAllKeys();
    void updatePlayerMoveState();
};