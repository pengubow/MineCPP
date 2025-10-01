#include <cmath>
#include <iostream>
#include "character/ZombieModel.h"

ZombieModel::ZombieModel() {
    this->head.addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    this->body = Cube(16, 16);
    this->body.addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4);
    this->arm0 = Cube(40, 16);
    this->arm0.addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4);
    this->arm0.setPos(-5.0f, 2.0f, 0.0f);
    this->arm1 = Cube(40, 16);
    this->arm1.addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4);
    this->arm1.setPos(5.0f, 2.0f, 0.0f);
    this->leg0 = Cube(0, 16);
    this->leg0.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    this->leg0.setPos(-2.0f, 12.0f, 0.0f);
    this->leg1 = Cube(0, 16);
    this->leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    this->leg1.setPos(2.0f, 12.0f, 0.0f);
}

void ZombieModel::render(double time) {
    this->head.yRot = sin(time * 0.83) * 1.0f;
    this->head.xRot = sin(time) * 0.8f;
    this->arm0.xRot = sin(time * 0.6662 + M_PI) * 2.0f;
    this->arm0.zRot = (sin(time * 0.2312) + 1.0) * 1.0f;
    this->arm1.xRot = sin(time * 0.6662) * 2.0f;
    this->arm1.zRot = (sin(time * 0.2812) - 1.0) * 1.0f;
    this->leg0.xRot = sin(time * 0.6662) * 1.4f;
    this->leg1.xRot = sin(time * 0.6662 + M_PI) * 1.4f;
    this->head.render();
    this->body.render();
    this->arm0.render();
    this->arm1.render();
    this->leg0.render();
    this->leg1.render();
}