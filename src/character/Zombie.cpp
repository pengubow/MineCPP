#include <GL/gl.h>
#include <cmath>
#include "character/Zombie.h"
#include "Timer.h"
#include "Util/Util.h"

ZombieModel Zombie::zombieModel;

Zombie::Zombie(shared_ptr<Level>& level, float x, float y, float z) 
    : Entity(level) {
    Zombie::zombieModel = ZombieModel();
    this->setPos(x, y, z);
    this->timeOffs = Util::nextFloat() * 1239813.0f;
    this->rot = (Util::nextFloat() * M_PI * 2.0);
    this->speed = 1.0f;
}

void Zombie::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    float xa = 0.0f;
    float ya = 0.0f;
    if (this->y < -100.0f) {
        this->remove();
    }
    this->rot += this->rotA;
    this->rotA = this->rotA * 0.99;
    this->rotA = (float)((double)rotA + (Util::random() - Util::random()) * Util::random() * Util::random() * (double)0.08f);
    xa = sin(this->rot);
    ya = cos(this->rot);
    if (this->onGround && Util::nextFloat() < 0.08) {
        this->yd = 0.5f;
    }
    this->moveRelative(xa, ya, this->onGround ? 0.1f : 0.02f);
    this->yd = this->yd - 0.08;
    this->move(this->xd, this->yd, this->zd);
    this->xd *= 0.91f;
    this->yd *= 0.98f;
    this->zd *= 0.91f;
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
    }
}

void Zombie::render(shared_ptr<Textures>& textures, float a) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char.png", GL_NEAREST));
    glPushMatrix();
    double time = Timer::nanoTime() / 1.0E9 * 10 * this->speed + this->timeOffs;
    float brightness = getBrightness();
	glColor3f(brightness, brightness, brightness);
    float size = 0.058333334f;
    float yy = -abs(sin(time * 0.6662)) * 5.0 - 23.0;
    glTranslatef(this->xo + (this->x - this->xo) * a, this->yo + (this->y - this->yo) * a, this->zo + (this->z - this->zo) * a);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(size, size, size);
    glTranslatef(0.0f, yy, 0.0f);
    float c = 57.29578f;
    glRotatef(this->rot * c + 180.0f, 0.0f, 1.0f, 0.0f);
    zombieModel.head.yRot = sin(time * 0.83) * 1.0f;
    zombieModel.head.xRot = sin(time) * 0.8f;
    zombieModel.arm0.xRot = sin(time * 0.6662 + M_PI) * 2.0f;
    zombieModel.arm0.zRot = (sin(time * 0.2312) + 1.0) * 1.0f;
    zombieModel.arm1.xRot = sin(time * 0.6662) * 2.0f;
    zombieModel.arm1.zRot = (sin(time * 0.2812) - 1.0) * 1.0f;
    zombieModel.leg0.xRot = sin(time * 0.6662) * 1.4f;
    zombieModel.leg1.xRot = sin(time * 0.6662 + M_PI) * 1.4f;
    zombieModel.head.render();
    zombieModel.body.render();
    zombieModel.arm0.render();
    zombieModel.arm1.render();
    zombieModel.leg0.render();
    zombieModel.leg1.render();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}