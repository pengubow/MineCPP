#include "level/Level.h"
#include "particle/Particle.h"
#include "renderer/Textures.h"
#include "Player.h"

class ParticleEngine {
protected:
    shared_ptr<Level> level;
private:
    vector<shared_ptr<Particle>> particles;
    shared_ptr<Textures> textures;
public:
    ParticleEngine(shared_ptr<Level>& level, shared_ptr<Textures>& textures);

    void add(shared_ptr<Particle>& p);
    void tick();
    void render(shared_ptr<Player>& player, float a, int32_t layer);
};