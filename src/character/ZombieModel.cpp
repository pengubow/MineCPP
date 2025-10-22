#include <cmath>
#include <iostream>
#include "character/ZombieModel.h"

ZombieModel::ZombieModel() {
    head.addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    body = Cube(16, 16);
    body.addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4);
    arm0 = Cube(40, 16);
    arm0.addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4);
    arm0.setPos(-5.0f, 2.0f, 0.0f);
    arm1 = Cube(40, 16);
    arm1.addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4);
    arm1.setPos(5.0f, 2.0f, 0.0f);
    leg0 = Cube(0, 16);
    leg0.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg0.setPos(-2.0f, 12.0f, 0.0f);
    leg1 = Cube(0, 16);
    leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg1.setPos(2.0f, 12.0f, 0.0f);
}

void ZombieModel::render(double time, float var2, float var3) {
    head.yRot = var2 * (M_PI / 180.0f);
    head.xRot = var3 * (M_PI / 180.0f);
    arm0.xRot = (float)sin(time * 0.6662 + M_PI) * 2.0f;
    arm0.zRot = (float)(sin(time * 0.2312) + 1.0);
    arm1.xRot = (float)sin(time * 0.6662) * 2.0f;
    arm1.zRot = (float)(sin(time * 0.2812) - 1.0);
    leg0.xRot = (float)sin(time * 0.6662) * 1.4f;
    leg1.xRot = (float)sin(time * 0.6662 + M_PI) * 1.4f;
    head.render();
    body.render();
    arm0.render();
    arm1.render();
    leg0.render();
    leg1.render();
}