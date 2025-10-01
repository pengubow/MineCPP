#include <GL/glu.h>
#include <cmath>
#include "particle/ParticleEngine.h"
#include "Textures.h"

ParticleEngine::ParticleEngine(std::shared_ptr<Level>& level) : level(level) {};

void ParticleEngine::add(std::shared_ptr<Particle>& p) {
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

void ParticleEngine::render(std::shared_ptr<Player>& player, float a, int32_t layer) {
    glEnable(GL_TEXTURE_2D);
    int32_t id = Textures::loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    float xa = -((float)std::cos((double)player->yRot * M_PI / 180.0));
    float za = -((float)std::sin((double)player->yRot * M_PI / 180.0));
    float xa2 = -za * (float)std::sin((double)player->xRot * M_PI / 180.0);
    float za2 = xa * (float)std::sin((double)player->xRot * M_PI / 180.0);
    float ya = (float)std::cos((double)player->xRot * M_PI / 180.0);
    std::shared_ptr<Tesselator> t = Tesselator::instance;
    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
    t->init();
    for (int32_t i = 0; i < this->particles.size(); i++) {
        std::shared_ptr<Particle> p = this->particles.at(i);
        if (p->isLit() ^ layer == 1) {
            p->render(t, a, xa, ya, za, xa2, za2);
        }
    }
    t->flush();
    glDisable(GL_TEXTURE_2D);
};