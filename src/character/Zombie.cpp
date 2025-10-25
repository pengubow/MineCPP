#include <cmath>
#include "GL_compat.h"
#include "character/Zombie.h"
#include "Timer.h"
#include "Util/Util.h"

ZombieModel Zombie::zombieModel = ZombieModel();

Zombie::Zombie(shared_ptr<Level>& level, float x, float y, float z) 
    : Entity(level) {
    setPos(x, y, z);
    timeOffs = Random::random() * 1239813.0f;
    rot = (Random::random() * M_PI * 2.0);
    speed = 1.0f;
}

void Zombie::tick() {
    xo = x;
    yo = y;
    zo = z;
    float xa = 0.0f;
    float ya = 0.0f;
    if (y < -100.0f) {
        remove();
    }
    rot += rotA;
    rotA = rotA * 0.99;
    rotA = (float)((double)rotA + (Random::random() - Random::random()) * Random::random() * Random::random() * (double)0.08f);
    xa = sin(rot);
    ya = cos(rot);
    if (onGround && Random::random() < 0.08) {
        yd = 0.5f;
    }
    moveRelative(xa, ya, onGround ? 0.1f : 0.02f);
    yd = yd - 0.08;
    move(xd, yd, zd);
    xd *= 0.91f;
    yd *= 0.98f;
    zd *= 0.91f;
    if (onGround) {
        xd *= 0.7f;
        zd *= 0.7f;
    }
}

void Zombie::render(shared_ptr<Textures>& textures, float a) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->getTextureId("char.png"));
    glPushMatrix();
    double time = Timer::nanoTime() / 1.0E9 * 10 * speed + timeOffs;
    float brightness = getBrightness();
	glColor3f(brightness, brightness, brightness);
    float size = 0.058333334f;
    float yy = -abs(sin(time * 0.6662)) * 5.0 - 23.0;
    glTranslatef(xo + (x - xo) * a, yo + (y - yo) * a, zo + (z - zo) * a);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(size, size, size);
    glTranslatef(0.0f, yy, 0.0f);
    float c = 57.29578f;
    glRotatef(rot * c + 180.0f, 0.0f, 1.0f, 0.0f);
    zombieModel.render(time, 0.0f, 0.0f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}