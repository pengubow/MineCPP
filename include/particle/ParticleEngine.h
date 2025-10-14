#include "level/Level.h"
#include "particle/Particle.h"
#include "renderer/Textures.h"
#include "player/Player.h"

class ParticleEngine {
public:
    vector<shared_ptr<Particle>> particles;
private:
    shared_ptr<Textures> textures;
public:
    ParticleEngine(shared_ptr<Level>& level, shared_ptr<Textures>& textures);

    void render(shared_ptr<Player>& player, float a);
};