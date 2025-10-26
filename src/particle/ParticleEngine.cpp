#include <iostream>
#include <cmath>
#include "GL_compat.h"
#include "particle/ParticleEngine.h"
#include "renderer/Textures.h"
#include "player/Player.h"

ParticleEngine::ParticleEngine(shared_ptr<Level>& level, shared_ptr<Textures>& textures) : textures(textures) {};

void ParticleEngine::render(Player* player, float a) {
    if (!this->particles.empty()) {
        glEnable(GL_TEXTURE_2D);
        int32_t id = textures->getTextureId("terrain.png");
        glBindTexture(GL_TEXTURE_2D, id);
        float xa = -((float)cos((double)player->yRot * M_PI / 180.0));
        float za = -((float)sin((double)player->yRot * M_PI / 180.0));
        float xa2 = -za * (float)sin((double)player->xRot * M_PI / 180.0);
        float za2 = xa * (float)sin((double)player->xRot * M_PI / 180.0);
        float ya = (float)cos((double)player->xRot * M_PI / 180.0);
        shared_ptr<Tesselator> t = Tesselator::instance;
        t->begin();

        for (int32_t var8 = 0; var8 < particles.size(); ++var8) {
            shared_ptr<Particle> var9 = particles[var8];
            float var10 = 0.8f * var9->getBrightness();
            t->color(var10, var10, var10);
            var9->render(t, a, xa, ya, za, xa2, za2);
        }

        t->end();
        glDisable(GL_TEXTURE_2D);
    }
};