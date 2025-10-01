#include "Entity.h"
#include "character/Cube.h"

class ZombieModel {
public:
    Cube head = Cube(0, 0);
    Cube body;
    Cube arm0;
    Cube arm1;
    Cube leg0;
    Cube leg1;

    ZombieModel();

    void render(double time);
};