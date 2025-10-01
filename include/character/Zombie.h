#include "Entity.h"
#include "character/ZombieModel.h"
#include "level/Level.h"
#include "Util.h"

class Zombie : public Entity {
public:
    float rot;
    float timeOffs;
    float speed;
    float rotA = (Util::randomfr() + 1.0f) * 0.01f;
private:
    static ZombieModel zombieModel;
public:
    Zombie(std::shared_ptr<Level>& level, float x, float y, float z);

    void tick() override;

    void render(float a);
};