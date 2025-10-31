#include "Minecraft.h"
#include "renderer/RenderHelper.h"

RenderHelper::RenderHelper(shared_ptr<Minecraft>& minecraft) 
    : minecraft(minecraft) {}

void RenderHelper::toggleLight(bool toggleLight) {
    if (!toggleLight) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
    else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        float var4 = 0.7f;
        float var2 = 0.3f;
        Vec3 var3 = Vec3(0.0f, -1.0f, 0.5f).normalize();
        glLightfv(GL_LIGHT0, GL_POSITION, getBuffer(var3.x, var3.y, var3.z, 0.0f).data());
        glLightfv(GL_LIGHT0, GL_DIFFUSE, getBuffer(var2, var2, var2, 1.0f).data());
        glLightfv(GL_LIGHT0, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f).data());
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(var4, var4, var4, 1.0f).data());
    }
}

void RenderHelper::initGui() {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    int32_t width = minecraft->width * 240 / minecraft->height;
    int32_t height = minecraft->height * 240 / minecraft->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
}

void RenderHelper::setupFog() {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    shared_ptr<Level> level = minecraft->level;
    shared_ptr<Player> player = minecraft->player;
    glFogfv(GL_FOG_COLOR, getBuffer(fogColorRed, fogColorGreen, fogColorBlue, 1.0f).data());
		glNormal3f(0.0f, -1.0f, 0.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		Tile* tile = Tile::tiles[level->getTile((int32_t)player->x, (int32_t)(player->y + 0.12f), (int32_t)player->z)];
		if(tile != nullptr && tile->getLiquidType() != Liquid::none) {
			Liquid* liquid = tile->getLiquidType();
			glFogi(GL_FOG_MODE, GL_EXP);
			if(liquid == Liquid::water) {
				glFogf(GL_FOG_DENSITY, 0.1f);
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.4f, 0.9f, 1.0f).data());
			} else if(liquid == Liquid::lava) {
				glFogf(GL_FOG_DENSITY, 2.0f);
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.3f, 0.3f, 1.0f).data());
			}
		} else {
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glFogf(GL_FOG_START, 0.0f);
			glFogf(GL_FOG_END, renderDistance);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f).data());
		}

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT);
		glEnable(GL_LIGHTING);
}

vector<float>& RenderHelper::getBuffer(float a, float b, float c, float d) {
    this->lb.clear();
    this->lb.reserve(4);
    this->lb.push_back(a);
    this->lb.push_back(b);
    this->lb.push_back(c);
    this->lb.push_back(d);
    return this->lb;
}