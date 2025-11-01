#include "gui/Gui.h"

void Gui::fill(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4) {
    float var5 = (float)((uint32_t)var4 >> 24) / 255.0F;
    float var6 = (float)(var4 >> 16 & 255) / 255.0F;
    float var7 = (float)(var4 >> 8 & 255) / 255.0F;
    float var9 = (float)(var4 & 255) / 255.0F;
    shared_ptr<Tesselator> t = Tesselator::instance;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(var6, var7, var9, var5);
    t->begin();
    t->vertex((float)var0, (float)var3, 0.0F);
    t->vertex((float)var2, (float)var3, 0.0F);
    t->vertex((float)var2, (float)var1, 0.0F);
    t->vertex((float)var0, (float)var1, 0.0F);
    t->end();
    glDisable(GL_BLEND);
}

void Gui::fillGradient(int32_t var0, int32_t var1, int32_t x, int32_t y, int32_t var4, int32_t var5) {
    float var10 = (float)((var4 >> 24) & 0xFF) / 255.0F;
    float var11 = (float)(var4 >> 16 & 255) / 255.0F;
    float var6 = (float)(var4 >> 8 & 255) / 255.0F;
    float var12 = (float)(var4 & 255) / 255.0F;
    float var7 = (float)((var5 >> 24) & 0xFF) / 255.0F;
    float var8 = (float)(var5 >> 16 & 255) / 255.0F;
    float var9 = (float)(var5 >> 8 & 255) / 255.0F;
    float var13 = (float)(var5 & 255) / 255.0F;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(var11, var6, var12, var10);
    glVertex2f((float)x, (float)var1);
    glVertex2f((float)var0, (float)var1);
    glColor4f(var8, var9, var13, var7);
    glVertex2f((float)var0, (float)y);
    glVertex2f((float)x, (float)y);
    glEnd();
    glDisable(GL_BLEND);
}

void Gui::drawCenteredString(shared_ptr<Font>& font, string msg, int32_t x, int32_t y, int32_t var4) {
    font->drawShadow(msg, x - font->width(msg) / 2, y, var4);
}

void Gui::drawString(shared_ptr<Font>& font, string var1, int32_t var2, int32_t var3, int32_t var4) {
    font->drawShadow(var1, var2, var3, var4);
}

void Gui::blit(int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5, int32_t var6) {
    float var7 = 0.00390625F;
    float var8 = 0.00390625F;
    shared_ptr<Tesselator> t = Tesselator::instance;
    t->begin();
    t->vertexUV((float)var1, (float)(var2 + var6), zLevel, (float)var3 * var7, (float)(var4 + var6) * var8);
    t->vertexUV((float)(var1 + var5), (float)(var2 + var6), zLevel, (float)(var3 + var5) * var7, (float)(var4 + var6) * var8);
    t->vertexUV((float)(var1 + var5), (float)var2, zLevel, (float)(var3 + var5) * var7, (float)var4 * var8);
    t->vertexUV((float)var1, (float)var2, zLevel, (float)var3 * var7, (float)var4 * var8);
    t->end();
}