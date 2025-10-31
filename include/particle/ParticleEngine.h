#pragma once

#include "level/Level.h"
#include "particle/Particle.h"
#include "renderer/Textures.h"

class Player;

class ParticleEngine {
public:
    vector<shared_ptr<Particle>> particles;
private:
    shared_ptr<Textures> textures;
public:
    ParticleEngine(shared_ptr<Level>& level, shared_ptr<Textures>& textures);

    void tick();
    void render(Player* player, float a);
};