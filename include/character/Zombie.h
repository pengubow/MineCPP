#include "Entity.h"
#include "character/ZombieModel.h"
#include "level/Level.h"
#include "renderer/Textures.h"
#include "Util/Util.h"

class Zombie : public Entity {
public:
    static const int64_t serialVersionUID = 77479605454997290;
private:
    static ZombieModel zombieModel;
public:
    float rot;
    float timeOffs;
    float speed;
    float rotA = (float)(Random::random() + 1.0) * 0.01f;

    Zombie(shared_ptr<Level>& level, float x, float y, float z);

    void tick() override;
    void render(shared_ptr<Textures>& textures, float a) override;
};