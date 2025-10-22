#pragma once

#include <stdint.h>
#include "character/ZombieModel.h"

class NetworkPlayer : public Entity {
public:
    static const int64_t serialVersionUID = 77479605454997290;
private:
    static ZombieModel playerModel;
    int32_t ticks = 0;
public:
    NetworkPlayer(shared_ptr<Level>& level, int32_t var2, string var3, float x, float y, float z, float yRot, float xRot);
    
    void tick();
    void render(shared_ptr<Textures>& textures, float var2);
};