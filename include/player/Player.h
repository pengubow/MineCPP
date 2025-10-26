#pragma once

#include <cstdint>
#include <memory>
#include "Entity.h"
#include "level/Level.h"
#include "player/MovementInputFromOptions.h"
#include "player/Inventory.h"

class Player : public Entity {
    shared_ptr<MovementInputFromOptions> input;
public:
    shared_ptr<Inventory> inventory = make_shared<Inventory>();
    int8_t userType = 0;

    Player(shared_ptr<Level>& level, shared_ptr<MovementInputFromOptions>& input);
    
    void tick() override;
    void releaseAllKeys();
    void setKey(int32_t key, bool state);
};