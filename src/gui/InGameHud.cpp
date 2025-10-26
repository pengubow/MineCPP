#include "gui/InGameHud.h"

InGameHud::InGameHud(shared_ptr<Minecraft>& minecraft, int32_t scaledWidth, int32_t scaledHeight) {
    this->minecraft = minecraft;
    this->scaledWidth = scaledWidth * 240 / scaledHeight;
    this->scaledHeight = scaledHeight * 240 / scaledHeight;
}

void InGameHud::render() {
    shared_ptr<Font> font = minecraft->font;
    minecraft->initGui();
    shared_ptr<Textures> textures = minecraft->textures;
    glBindTexture(GL_TEXTURE_2D, textures->getTextureId("gui.png"));
    glEnable(GL_TEXTURE_2D);
    shared_ptr<Tesselator> t = Tesselator::instance;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    blit(scaledWidth / 2 - 91, scaledHeight - 22, 0, 0, 182, 22);
    int32_t var5 = 0;

    int32_t var10001;
    while (true) {
        if (var5 >= User::getCreativeTiles().size()) {
            var10001 = 0;
            break;
        }

        if (User::getCreativeTiles()[var5] == minecraft->paintTexture) {
            var10001 = var5;
            break;
        }

        var5++;
    }

    blit((scaledWidth / 2 - 91 - 1) + var10001 * 20, scaledHeight - 22 - 1, 0, 22, 24, 22);
    glDisable(GL_BLEND);

    int var13;
    for (var13 = 0; var13 < 9; ++var13) {
        var5 = User::getCreativeTiles()[var13];
        glPushMatrix();
        glTranslatef((float)(scaledWidth / 2 - 90 + var13 * 20), (float)(scaledHeight - 16), -50.0F);
        glScalef(10.0f, 10.0f, 10.0f);
        glTranslatef(1.0f, 0.5f, 0.0f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-1.5f, 0.5f, 0.5f);
        glScalef(-1.0f, -1.0f, -1.0f);
        int32_t id = textures->getTextureId("terrain.png");
        glBindTexture(GL_TEXTURE_2D, id);
        glEnable(GL_TEXTURE_2D);
        t->begin();
        Tile::tiles[var5]->render(t, minecraft->level, 0, -2, 0, 0);
        t->end();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    font->drawShadow("0.0.19a_06", 2, 2, 16777215);
    font->drawShadow(minecraft->fpsString, 2, 12, 16777215);
    uint8_t var14 = 10;
    bool var15 = false;
    if (dynamic_pointer_cast<ChatScreen>(minecraft->screen)) {
        var14 = 20;
        var15 = true;
    }

    for (int32_t var6 = 0; var6 < messages.size() && var6 < var14; ++var6) {
        if (messages[var6].counter < 200 || var15) {
            font->drawShadow((messages[var6]).message, 2, scaledHeight - 8 - (var6 << 3) - 16, 16777215);
        }
    }

    int32_t var6 = scaledWidth / 2;
    int32_t var9 = scaledHeight / 2;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    t->begin();
    t->vertex((float)(var6 + 1), (float)(var9 - 4), 0.0f);
    t->vertex((float)var6, (float)(var9 - 4), 0.0f);
    t->vertex((float)var6, (float)(var9 + 5), 0.0f);
    t->vertex((float)(var6 + 1), (float)(var9 + 5), 0.0f);
    t->vertex((float)(var6 + 5), (float)var9, 0.0f);
    t->vertex((float)(var6 - 4), (float)var9, 0.0f);
    t->vertex((float)(var6 - 4), (float)(var9 + 1), 0.0f);
    t->vertex((float)(var6 + 5), (float)(var9 + 1), 0.0f);
    t->end();
    if(Util::isKeyDown(GLFW_KEY_TAB) && minecraft->connectionManager != nullptr && minecraft->connectionManager->isConnected()) {
        ConnectionManager* connectionManager = minecraft->connectionManager;
        vector<string> playerNames;
        playerNames.push_back(minecraft->user->name);
        for (auto& pair : connectionManager->players) {
            auto var14 = pair.second;
            playerNames.push_back(var14->name);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glVertex2f((float)(var6 + 128), (float)(var9 - 68 - 12));
        glVertex2f((float)(var6 - 128), (float)(var9 - 68 - 12));
        glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
        glVertex2f((float)(var6 - 128), (float)(var9 + 68));
        glVertex2f((float)(var6 + 128), (float)(var9 + 68));
        glEnd();
        glDisable(GL_BLEND);
        string var11 = "Connected players:";
        font->drawShadow(var11, var6 - font->width(var11) / 2, var9 - 64 - 12, 16777215);

        for(int32_t var12 = 0; var12 < playerNames.size(); ++var12) {
            var13 = var6 + var12 % 2 * 120 - 120;
            var5 = var9 - 64 + (var12 / 2 << 3);
            font->draw(playerNames[var12], var13, var5, 16777215);
        }
    }
}

void InGameHud::blit(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5) {
    float var7 = 0.00390625f;
    float var8 = 0.015625f;
    shared_ptr<Tesselator> t = Tesselator::instance;
    t->begin();
    t->vertexUV((float)var0, (float)(var1 + 22), -90.0f, 0.0f, (float)(var3 + 22) * var8);
    t->vertexUV((float)(var0 + var4), (float)(var1 + 22), -90.0f, (float)(var4 + 0) * var7, (float)(var3 + 22) * var8);
    t->vertexUV((float)(var0 + var4), (float)var1, -90.0f, (float)(var4 + 0) * var7, (float)var3 * var8);
    t->vertexUV((float)var0, (float)var1, -90.0f, 0.0f, (float)var3 * var8);
    t->end();
}