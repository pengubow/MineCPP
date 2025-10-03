#include "Entity.h"
#include "level/Level.h"
#include "renderer/Tesselator.h"

class Particle : public Entity {
private:
    float xd;
    float yd;
    float zd;
public:
    int32_t tex;
private:
    float uo;
    float vo;
    int32_t age = 0;
    int32_t lifetime = 0;
    float size;
public:
    Particle(shared_ptr<Level>& level, float x, float y, float z, float xa, float ya, float za, int32_t tex);

    void tick();
    void render(shared_ptr<Tesselator>& t, float a, float xa, float ya, float za, float xa2, float za2);
};