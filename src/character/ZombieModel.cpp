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

void ZombieModel::render(double time, float var2, float var3, float var4, float var5) {
    head.yRot = var4 / 57.29578f;
    head.xRot = var5 / 57.29578f;
    arm0.xRot = (float)cos((double)time * 0.6662 + M_PI) * 2.0f * var2;
    arm0.zRot = (float)(cos((double)time * 0.2312) + 1.0) * var2;
    arm1.xRot = (float)cos((double)time * 0.6662) * 2.0f * var2;
    arm1.zRot = (float)(cos((double)time * 0.2812) - 1.0) * var2;
    leg0.xRot = (float)cos((double)time * 0.6662) * 1.4f * var2;
    leg1.xRot = (float)cos((double)time * 0.6662 + M_PI) * 1.4f * var2;
    arm0.zRot += (float)cos((double)var3 * 0.09) * 0.05f + 0.05f;
    arm1.zRot -= (float)cos((double)var3 * 0.09) * 0.05f + 0.05f;
    arm0.xRot += (float)sin((double)var3 * 0.067) * 0.05f;
    arm1.xRot -= (float)sin((double)var3 * 0.067) * 0.05f;
    head.render();
    body.render();
    arm0.render();
    arm1.render();
    leg0.render();
    leg1.render();
}