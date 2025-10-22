#include "net/NetworkPlayer.h"

ZombieModel NetworkPlayer::playerModel = ZombieModel();

NetworkPlayer::NetworkPlayer(shared_ptr<Level>& level, int32_t var2, string var3, float x, float y, float z, float yRot, float xRot) 
    : Entity(level) {
    setPos(x, y, z);
    this->xRot = xRot;
    this->yRot = yRot;
    heightOffset = 1.62f;
}

void NetworkPlayer::tick() {
    xo = x;
    yo = y;
    zo = z;
    ticks++;
}

void NetworkPlayer::render(shared_ptr<Textures>& textures, float var2) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char.png", GL_NEAREST));
    glPushMatrix();
    float var5 = ((float)ticks + var2) / 2.0f;
    float var3 = getBrightness();
    glColor3f(var3, var3, var3);
    var3 = 0.058333334f;
    float var4 = (float)(-abs(sin((double)var5 * 0.6662)) * 5.0 - 23.0);
    glTranslatef(xo + (x - xo) * var2, yo + (y - yo) * var2 - heightOffset, zo + (z - zo) * var2);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(var3, var3, var3);
    glTranslatef(0.0f, var4, 0.0f);
    glRotatef(-yRot, 0.0f, 1.0f, 0.0f);
    playerModel.render(var5, xRot, 0.0f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}