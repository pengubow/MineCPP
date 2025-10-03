#include "Entity.h"
#include "character/ZombieModel.h"
#include "level/Level.h"
#include "renderer/Textures.h"
#include "Util.h"

class Zombie : public Entity {
public:
    float rot;
    float timeOffs;
    float speed;
    float rotA;
    shared_ptr<Textures> textures;
private:
    static ZombieModel zombieModel;
public:
    Zombie(shared_ptr<Level>& level, shared_ptr<Textures>& textures, float x, float y, float z);

    void tick();

    void render(float a) override;
};