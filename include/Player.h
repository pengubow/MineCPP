#pragma once

#include <cstdint>
#include <memory>
#include "Entity.h"
#include "level/Level.h"

class Player : public Entity {
public:
    Player(shared_ptr<Level>& level);
    
    void tick();
};