#pragma once

#include "Entity.h"
#include "character/Cube.h"

class ZombieModel {
private:
    Cube head = Cube(0, 0);
    Cube body;
    Cube arm0;
    Cube arm1;
    Cube leg0;
    Cube leg1;
public:
    ZombieModel();

    void render(double time, float var2, float var3);
};