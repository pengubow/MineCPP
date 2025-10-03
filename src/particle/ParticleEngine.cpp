#include <GL/glu.h>
#include <cmath>
#include "particle/ParticleEngine.h"
#include "renderer/Textures.h"

ParticleEngine::ParticleEngine(shared_ptr<Level>& level, shared_ptr<Textures>& textures) : level(level), textures(textures) {};

void ParticleEngine::add(shared_ptr<Particle>& p) {
    this->particles.push_back(p);
};

void ParticleEngine::tick() {
    for (int32_t i = 0; i < this->particles.size(); i++) {
        shared_ptr<Particle> p = this->particles.at(i);
        p->tick();
        if (p->removed) {
            this->particles.erase(this->particles.begin() + i);
            i--;
        }
    }
};

void ParticleEngine::render(shared_ptr<Player>& player, float a, int32_t layer) {
    if (this->particles.empty()) {
        return;
    }
    glEnable(GL_TEXTURE_2D);
    int32_t id = textures->loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    float xa = -((float)cos((double)player->yRot * M_PI / 180.0));
    float za = -((float)sin((double)player->yRot * M_PI / 180.0));
    float xa2 = -za * (float)sin((double)player->xRot * M_PI / 180.0);
    float za2 = xa * (float)sin((double)player->xRot * M_PI / 180.0);
    float ya = (float)cos((double)player->xRot * M_PI / 180.0);
    shared_ptr<Tesselator> t = Tesselator::instance;
    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
    t->init();
    for (int32_t i = 0; i < this->particles.size(); i++) {
        shared_ptr<Particle> p = this->particles.at(i);
        if (p->isLit() ^ layer == 1) {
            p->render(t, a, xa, ya, za, xa2, za2);
        }
    }
    t->flush();
    glDisable(GL_TEXTURE_2D);
};