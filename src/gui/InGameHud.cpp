#include "gui/InGameHud.h"

InGameHud::InGameHud(shared_ptr<Minecraft>& minecraft, int32_t scaledWidth, int32_t scaledHeight) {
    this->minecraft = minecraft;
    this->scaledWidth = scaledWidth * 240 / scaledHeight;
    this->scaledHeight = scaledHeight * 240 / scaledHeight;
}

void InGameHud::render(bool var1, int32_t var2, int32_t var3) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    shared_ptr<Font> font = minecraft->font;
    minecraft->renderHelper->initGui();
    shared_ptr<Textures> var5 = minecraft->textures;
    glBindTexture(GL_TEXTURE_2D, minecraft->textures->getTextureId("resources/textures/gui.png"));
    glEnable(GL_TEXTURE_2D);
    shared_ptr<Tesselator> t = Tesselator::instance;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    shared_ptr<Inventory> var7 = minecraft->player->inventory;
    zLevel = -90.0f;
    blit(scaledWidth / 2 - 91, scaledHeight - 22, 0, 0, 182, 22);
    blit(scaledWidth / 2 - 91 - 1 + var7->selectedSlot * 20, scaledHeight - 22 - 1, 0, 22, 24, 22);
    glDisable(GL_BLEND);

    for (int32_t var8 = 0; var8 < var7->slots.size(); ++var8) {
        int32_t slot = var7->slots[var8];
        if (slot > 0) {
            glPushMatrix();
            glTranslatef((float)(scaledWidth / 2 - 90 + var8 * 20), (float)(scaledHeight - 16), -50.0f);
            glScalef(10.0f, 10.0f, 10.0f);
            glTranslatef(1.0f, 0.5f, 0.0f);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(-1.5f, 0.5f, 0.5f);
            glScalef(-1.0f, -1.0f, -1.0f);
            int32_t var10 = var5->getTextureId("resources/textures/terrain.png");
            glBindTexture(GL_TEXTURE_2D, var10);
            glEnable(GL_TEXTURE_2D);
            t->begin();
            Tile::tiles[slot]->render(t, minecraft->level, 0, -2, 0, 0);
            t->end();
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        }
    }

    font->drawShadow("0.0.23a_01", 2, 2, 16777215);
    if (minecraft->options->showFPS) {
        font->drawShadow(minecraft->fpsString, 2, 12, 16777215);
    }

    uint8_t var18 = 10;
    bool var19 = false;
    if (dynamic_pointer_cast<ChatScreen>(minecraft->screen)) {
        var18 = 20;
        var19 = true;
    }

    for (int32_t var10 = 0; var10 < messages.size() && var10 < var18; var10++) {
        if (messages[var10].counter < 200 || var19) {
            font->drawShadow(messages[var10].message, 2, scaledHeight - 8 - var10 * 9 - 20, 16777215);
        }
    }

    int32_t var10 = scaledWidth / 2;
    int32_t var11 = scaledHeight / 2;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    t->begin();
    t->vertex((float)(var10 + 1), (float)(var11 - 4), 0.0f);
    t->vertex((float)var10, (float)(var11 - 4), 0.0f);
    t->vertex((float)var10, (float)(var11 + 5), 0.0f);
    t->vertex((float)(var10 + 1), (float)(var11 + 5), 0.0f);
    t->vertex((float)(var10 + 5), (float)var11, 0.0f);
    t->vertex((float)(var10 - 4), (float)var11, 0.0f);
    t->vertex((float)(var10 - 4), (float)(var11 + 1), 0.0f);
    t->vertex((float)(var10 + 5), (float)(var11 + 1), 0.0f);
    t->end();

    hoveredUsername = "";
    if (Util::isKeyDown(GLFW_KEY_TAB) && minecraft->connectionManager != nullptr && minecraft->connectionManager->isConnected()) {
        ConnectionManager* var12 = minecraft->connectionManager;
        vector<string> playerNames;
        playerNames.push_back(minecraft->user->name);
        for (auto& pair : var12->players) {
            auto var14 = pair.second;
            playerNames.push_back(var14->name);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glVertex2f((float)(var10 + 128), (float)(var11 - 68 - 12));
        glVertex2f((float)(var10 - 128), (float)(var11 - 68 - 12));
        glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
        glVertex2f((float)(var10 - 128), (float)(var11 + 68));
        glVertex2f((float)(var10 + 128), (float)(var11 + 68));
        glEnd();
        glDisable(GL_BLEND);
        string var16 = "Connected players:";
        font->drawShadow(var16, var10 - font->width(var16) / 2, var11 - 64 - 12, 16777215);

        for (int32_t var17 = 0; var17 < playerNames.size(); ++var17) {
            int32_t var8 = var10 + var17 % 2 * 120 - 120;
            int32_t var9 = var11 - 64 + (var17 / 2 << 3);
            if (var1 && var2 >= var8 && var3 >= var9 && var2 < var8 + 120 && var3 < var9 + 8) {
                hoveredUsername = playerNames[var17];
                font->draw(playerNames[var17], var8 + 2, var9, 16777215);
            }
            else {
                font->draw(playerNames[var17], var8, var9, 15658734);
            }
        }
    }

}

void InGameHud::addChatMessage(string message) {
    messages.push_front(ChatLine(message));

    while (messages.size() > 50) {
        messages.pop_back();
    }
}