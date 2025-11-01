#include <cmath>
#include "net/NetworkPlayer.h"
#include "net/NetworkSkinDownloadThread.h"
#include "Minecraft.h"
#include "renderer/Textures.h"
#include "gui/Font.h"
#include "GL_compat.h"

NetworkPlayer::NetworkPlayer(shared_ptr<Minecraft>& minecraft, int32_t id, string name, int32_t x, int32_t y, int32_t z, float yRot, float xRot)
    : Entity(minecraft->level), minecraft(minecraft), displayName(name), 
    name(Font::removeColorCodes(name)), xp(x), yp(y), zp(z) {
    zombieModel = minecraft->playerModel;
    displayName = name;
    setPos((float)x / 32.0f, (float)y / 32.0f, (float)z / 32.0f);
    this->xRot = xRot;
    this->yRot = yRot;
    make_shared<NetworkSkinDownloadThread>(this)->start();
}

void NetworkPlayer::tick() {
    Entity::tick();
    animStepO = animStep;
    yBodyRotO = yBodyRot;
    yRotO = yRot;
    xRotO = xRot;
    tickCount++;
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
    float var3 = (float)sqrt((double)(var6 * var6 + var2 * var2));
    float var4 = yBodyRot;
    float var5 = 0.0f;
    oRun = run;
    float whattonamethis = 0.0f;
    
    if (var3 != 0.0f) {
        whattonamethis = 1.0f;
        var5 = var3 * 3.0f;
        var4 = -((float)atan2((double)var2, (double)var6) * 180.0f / M_PI + 90.0f);
    }

    run += (whattonamethis - run) * 0.3f;

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
    yBodyRot += var6 * 0.1f;
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
    this->textures = textures;
    float var3 = oRun + (run - oRun) * a;
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    if (newTexture) {
        skin = textures->addTexture(newTexture, skinWidth, skinHeight);
        newTexture = nullptr;
    }

    if (skin < 0) {
        glBindTexture(GL_TEXTURE_2D, textures->getTextureId("resources/textures/char.png"));
    }
    else {
        glBindTexture(GL_TEXTURE_2D, skin);
    }
    
    while (yBodyRotO - yBodyRot < -180.0f) {
        yBodyRotO += 360.0f;
    }

    while (yBodyRotO - yBodyRot >= 180.0f) {
        yBodyRotO -= 360.0f;
    }

    float var9;
    for (var9 = yBodyRotO + (yBodyRot - yBodyRotO) * a; xRotO - xRot < -180.0f; xRotO += 360.0f) {}

    while (xRotO - xRot >= 180.0f) {
        xRotO -= 360.0f;
    }

    while (yRotO - yRot < -180.0f) {
        yRotO += 360.0f;
    }

    while (yRotO - yRot >= 180.0f) {
        yRotO -= 360.0f;
    }

    float var4 = yRotO + (yRot - yRotO) * a;
    float var5 = xRotO + (xRot - xRotO) * a;
    var4 = -(var4 - var9);  
    glPushMatrix();
    float var6 = animStepO + (animStep - animStepO) * a;
    float var7 = getBrightness();
    glColor3f(var7, var7, var7);
    var7 = 1.0F / 16.0F;
    float var8 = (float)(-abs(cos((double)var6 * 0.6662)) * 5.0 * (double)var3 - 23.0);
    glTranslatef(xo + (x - xo) * a, yo + (y - yo) * a - 1.62f, 
        zo + (z - zo) * a);
    glScalef(1.0f, -1.0f, 1.0f);
    glTranslatef(0.0f, var8 * var7, 0.0f);
    glRotatef(var9, 0.0f, 1.0f, 0.0f);
    glDisable(GL_ALPHA_TEST);
    glScalef(-1.0f, 1.0f, 1.0f);
    zombieModel.render(var6, var3, (float)tickCount + a, var4, var5, var7);
    glEnable(GL_ALPHA_TEST);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(xo + (x - xo) * a, yo + (y - yo) * a + 0.8f, 
        zo + (z - zo) * a);
    glRotatef(-minecraft->player->yRot, 0.0f, 1.0f, 0.0f);
    
    a = 0.05f;
    glScalef(a, -a, a);
    glTranslatef((float)(-minecraft->font->width(displayName)) / 2.0f, 0.0f, 0.0f);
    glNormal3f(1.0f, -1.0f, 1.0f);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_BUFFER_BIT);
    if (name == "Notch") {
        minecraft->font->draw(displayName, 0, 0, 16776960);
    } else {
        minecraft->font->draw(displayName, 0, 0, 16777215);
    }

    glEnable(GL_COLOR_BUFFER_BIT);
    glEnable(GL_LIGHTING);
    glTranslatef(1.0f, 1.0f, -0.05f);
    minecraft->font->draw(name, 0, 0, 5263440);
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void NetworkPlayer::queue(int8_t dx, int8_t dy, int8_t dz, float yRot, float xRot) {
    float var6 = yRot - this->yRot;

    float var7;
    for (var7 = xRot - this->xRot; var6 >= 180.0f; var6 -= 360.0f) {
    }

    while (var6 < -180.0f) {
        var6 += 360.0f;
    }

    while (var7 >= 180.0f) {
        var7 -= 360.0f;
    }

    while (var7 < -180.0f) {
        var7 += 360.0f;
    }

    var6 = this->yRot + var6 * 0.5f;
    var7 = this->xRot + var7 * 0.5f;
    moveQueue.push_back(PlayerMove(((float)xp + (float)dx / 2.0f) / 32.0f, ((float)yp + (float)dy / 2.0f) / 32.0f, ((float)zp + (float)dz / 2.0f) / 32.0f, var6, var7));
    xp += dx;
    yp += dy;
    zp += dz;
    moveQueue.push_back(PlayerMove((float)xp / 32.0f, (float)yp / 32.0f, (float)zp / 32.0f, yRot, xRot));
}

void NetworkPlayer::teleport(int16_t x, int16_t y, int16_t z, float yRot, float xRot) {
    float var6 = yRot - this->yRot;

    float var7;
    for (var7 = xRot - this->xRot; var6 >= 180.0f; var6 -= 360.0f) {
    }

    while (var6 < -180.0f) {
        var6 += 360.0f;
    }

    while (var7 >= 180.0f) {
        var7 -= 360.0f;
    }

    while (var7 < -180.0f) {
        var7 += 360.0f;
    }

    var6 = this->yRot + var6 * 0.5f;
    var7 = this->xRot + var7 * 0.5f;
    moveQueue.push_back(PlayerMove((float)(xp + x) / 64.0f, (float)(yp + y) / 64.0f, (float)(zp + z) / 64.0f, var6, var7));
    xp = x;
    yp = y;
    zp = z;
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
    float var3 = yRot - this->yRot;

    float var4;
    for (var4 = xRot - this->xRot; var3 >= 180.0F; var3 -= 360.0F) {
    }

    while (var3 < -180.0F) {
        var3 += 360.0F;
    }

    while (var4 >= 180.0F) {
        var4 -= 360.0F;
    }

    while (var4 < -180.0F) {
        var4 += 360.0F;
    }

    var3 = this->yRot + var3 * 0.5F;
    var4 = this->xRot + var4 * 0.5F;
    moveQueue.push_back(PlayerMove(var3, var4));
    moveQueue.push_back(PlayerMove(yRot, xRot));
}

void NetworkPlayer::clear() {
    if (skin >= 0) {
        cout << "Releasing texture for " << this->name << endl;
        textures->idBuffer.clear();
        textures->idBuffer.reserve(1);
        textures->idBuffer.push_back(skin);
        glDeleteTextures(1, textures->idBuffer.data());
    }
}