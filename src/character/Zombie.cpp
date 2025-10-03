#include <GL/gl.h>
#include <cmath>
#include "character/Zombie.h"
#include "Timer.h"
#include "Util.h"

ZombieModel Zombie::zombieModel;

Zombie::Zombie(shared_ptr<Level>& level, shared_ptr<Textures>& textures, float x, float y, float z) 
    : Entity(level) {
    Zombie::zombieModel = ZombieModel();
    this->textures = textures;
    this->rotA = (float)(Util::nextFloat() + 1.0) * 0.01f;
    this->setPos(x, y, z);
    this->timeOffs = Util::nextFloat() * 1239813.0f;
    this->rot = (Util::nextFloat() * M_PI * 2.0);
    this->speed = 1.0f;
}

void Zombie::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    if (this->y < -100.0f) {
        this->remove();
    }
    float xa = 0.0f;
    float ya = 0.0f;
    this->rot += this->rotA;
    this->rotA = this->rotA * 0.99;
    this->rotA = this->rotA + (Util::nextFloat() - Util::nextFloat()) * Util::nextFloat() * Util::nextFloat() * 0.08f;
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

void Zombie::render(float a) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char.png", GL_NEAREST));
    glPushMatrix();
    double time = Timer::nanoTime() / 1.0E9 * 10 * this->speed + this->timeOffs;
    float size = 0.058333334f;
    float yy = -abs(sin(time * 0.6662)) * 5.0 - 23.0;
    glTranslatef(this->xo + (this->x - this->xo) * a, this->yo + (this->y - this->yo) * a, this->zo + (this->z - this->zo) * a);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(size, size, size);
    glTranslatef(0.0f, yy, 0.0f);
    float c = 57.29578f;
    glRotatef(this->rot * c + 180.0f, 0.0f, 1.0f, 0.0f);
    zombieModel.render(time);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}