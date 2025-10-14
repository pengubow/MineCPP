#pragma once

#include <cstdint>
#include <memory>
#include "Entity.h"
#include "level/Level.h"
#include "player/MovementInputFromOptions.h"

class Player : public Entity {
private:
    shared_ptr<Entity> entity;
    shared_ptr<MovementInputFromOptions> input;
public:
    Player(shared_ptr<Level>& level, shared_ptr<MovementInputFromOptions>& input);
    
    void tick() override;
    void releaseAllKeys();
    void setKey(int32_t key, bool state);
};