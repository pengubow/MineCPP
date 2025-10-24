#include <cmath>
#include "net/NetworkPlayer.h"
#include "Minecraft.h"
#include "renderer/Textures.h"
#include "gui/Font.h"
#include "GL_compat.h"

NetworkPlayer::NetworkPlayer(shared_ptr<Minecraft>& minecraft, int32_t id, string name, int32_t x, int32_t y, int32_t z, float yRot, float xRot)
    : Entity(minecraft->level), minecraft(minecraft), name(name), 
    xp(x), yp(y), zp(z) {
    zombieModel = minecraft->playerModel;
    setPos((float)x / 32.0f, (float)y / 32.0f, (float)z / 32.0f);
    this->xRot = xRot;
    this->yRot = yRot;
    this->heightOffset = 1.62f;
}

void NetworkPlayer::tick() {
    Entity::tick();
    animStepO = animStep;
    int32_t var1 = 5;

    do {
        if (moveQueue.size() > 0) {
            PlayerMove move = moveQueue.front();
            moveQueue.pop_front();
            setPos(move);
        }
    } while (var1-- > 0 && moveQueue.size() > 10);

    float var6 = x - xo;
    float var2 = z - zo;
    yBodyRotO = yBodyRot;
    float var3 = (float)sqrt((double)(var6 * var6 + var2 * var2));
    float var4 = yBodyRot;
    float var5 = 0.0f;
    
    if (var3 == 0.0f) {
        animStep = 0.0f;
    } else {
        var5 = var3 * 3.0f;
        var4 = -((float)atan2((double)var2, (double)var6) * 180.0f / M_PI + 90.0f);
    }

    for (var6 = var4 - yBodyRot; var6 < -180.0f; var6 += 360.0f) {}

    while (var6 >= 180.0f) {
        var6 -= 360.0f;
    }

    yBodyRot += var6 * 0.1f;

    for (var6 = yRot - yBodyRot; var6 < -180.0f; var6 += 360.0f) {}

    while (var6 >= 180.0f) {
        var6 -= 360.0f;
    }

    bool var7 = var6 < -90.0f || var6 >= 90.0f;
    if (var6 < -75.0f) {
        var6 = -75.0f;
    }
    if (var6 >= 75.0f) {
        var6 = 75.0f;
    }

    yBodyRot = yRot - var6;
    if (var7) {
        var5 = -var5;
    }

    while (yRot - yRotO < -180.0f) {
        yRotO -= 360.0f;
    }

    while (yRot - yRotO >= 180.0f) {
        yRotO += 360.0f;
    }

    while (yBodyRot - yBodyRotO < -180.0f) {
        yBodyRotO -= 360.0f;
    }

    while (yBodyRot - yBodyRotO >= 180.0f) {
        yBodyRotO += 360.0f;
    }

    while (xRot - xRotO < -180.0f) {
        xRotO -= 360.0f;
    }

    while (xRot - xRotO >= 180.0f) {
        xRotO += 360.0f;
    }

    animStep += var5;
}

void NetworkPlayer::render(shared_ptr<Textures>& textures, float a) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char.png", GL_NEAREST));
    
    float var8 = yBodyRotO + (yBodyRot - yBodyRotO) * a;
    float var3 = yRotO + (yRot - yRotO) * a;
    float var4 = xRotO + (xRot - xRotO) * a;
    var3 -= var8;
    
    glPushMatrix();
    float var5 = animStepO + (animStep - animStepO) * a;
    float var6 = getBrightness();
    glColor3f(var6, var6, var6);
    var6 = 1.0f / 16.0f;
    float var7 = (float)(-fabs(sin((double)var5 * 0.6662)) * 5.0 - 23.0);
    
    glTranslatef(xo + (x - xo) * a, yo + (y - yo) * a - heightOffset, 
                 zo + (z - zo) * a);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(var6, var6, var6);
    glTranslatef(0.0f, var7, 0.0f);
    glRotatef(var8, 0.0f, 1.0f, 0.0f);
    zombieModel.render(var5, var3, var4);
    
    glPopMatrix();
    glPushMatrix();
    glTranslatef(xo + (x - xo) * a, yo + (y - yo) * a + 0.8f, 
                 zo + (z - zo) * a);
    glRotatef(-minecraft->player->yRot, 0.0f, 1.0f, 0.0f);
    
    a = 0.05f;
    glScalef(a, -a, a);
    glTranslatef((float)(-minecraft->font->width(name)) / 2.0f, 0.0f, 0.0f);
    
    if (name == "Notch") {
        minecraft->font->draw(name, 0, 0, 16776960);
    } else {
        minecraft->font->draw(name, 0, 0, 16777215);
    }

    glTranslatef(1.0f, 1.0f, -0.05f);
    minecraft->font->draw(name, 0, 0, 5263440);
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void NetworkPlayer::queue(int8_t dx, int8_t dy, int8_t dz, float yRot, float xRot) {
    moveQueue.push_back(PlayerMove(((float)xp + (float)dx / 2.0f) / 32.0f, ((float)yp + (float)dy / 2.0f) / 32.0f, ((float)zp + (float)dz / 2.0f) / 32.0f, (this->yRot + yRot) / 2.0f, (this->xRot + xRot) / 2.0f));
    xp += dx;
    yp += dy;
    zp += dz;
    moveQueue.push_back(PlayerMove((float)xp / 32.0f, (float)yp / 32.0f, (float)zp / 32.0f, yRot, xRot));
}

void NetworkPlayer::queue(int8_t dx, int8_t dy, int8_t dz) {
    moveQueue.push_back(PlayerMove(((float)xp + (float)dx / 2.0f) / 32.0f, ((float)yp + (float)dy / 2.0f) / 32.0f, ((float)zp + (float)dz / 2.0f) / 32.0f));
    xp += dx;
    yp += dy;
    zp += dz;
    moveQueue.push_back(PlayerMove((float)xp / 32.0f, (float)yp / 32.0f, (float)zp / 32.0f));
}

void NetworkPlayer::queue(float yRot, float xRot) {
    moveQueue.push_back(PlayerMove((this->yRot + yRot) / 2.0f, (this->xRot + xRot) / 2.0f));
    moveQueue.push_back(PlayerMove(yRot, xRot));
}

void NetworkPlayer::teleport(int16_t x, int16_t y, int16_t z, float yRot, float xRot) {
    moveQueue.push_back(PlayerMove((float)(xp + x) / 64.0f, (float)(yp + y) / 64.0f, (float)(zp + z) / 64.0f, (this->yRot + yRot) / 2.0f, (this->xRot + xRot) / 2.0f));
    xp = x;
    yp = y;
    zp = z;
    moveQueue.push_back(PlayerMove((float)xp / 32.0f, (float)yp / 32.0f, (float)zp / 32.0f, yRot, xRot));
}