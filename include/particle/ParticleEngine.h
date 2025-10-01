#include "level/Level.h"
#include "particle/Particle.h"
#include "Player.h"

class ParticleEngine {
protected:
    std::shared_ptr<Level> level;
private:
    std::vector<std::shared_ptr<Particle>> particles;
public:

    ParticleEngine(std::shared_ptr<Level>& level);

    void add(std::shared_ptr<Particle>& p);
    void tick();
    void render(std::shared_ptr<Player>& player, float a, int32_t layer);
};