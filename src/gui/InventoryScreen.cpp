#include "gui/InventoryScreen.h"
#include "User.h"
#include "Minecraft.h"

int32_t InventoryScreen::getTileAtSlot(int32_t var1, int32_t var2) {
    for (int32_t i = 0; i < User::getCreativeTiles().size(); i++) {
        int32_t var4 = width / 2 + i % 8 * 24 - 96;
        int32_t var5 = height / 2 + i / 8 * 24 - 48;
        if (var1 >= var4 && var1 <= var4 + 24 && var2 >= var5 - 12 && var2 <= var5 + 12) {
            return i;
        }
    }
    return -1;
}

void InventoryScreen::render(int32_t var1, int32_t var2) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString("Select block", width / 2, 40, 16777215);
    shared_ptr<Textures> textures = minecraft->textures;
    shared_ptr<Tesselator> t = Tesselator::instance;
    int32_t tile = getTileAtSlot(var1, var2);
    int32_t id = textures->getTextureId("terrain.png");
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable(GL_TEXTURE_2D);

    for (int32_t i = 0; i < User::getCreativeTiles().size(); i++) {
        Tile* tile1 = User::getCreativeTiles()[i];
        glPushMatrix();
        int32_t var5 = width / 2 + i % 8 * 24 - 96;
        int32_t var6 = height / 2 + i / 8 * 24 - 48;
        glTranslatef((float)var5, (float)var6, 0.0f);
        glScalef(10.0f, 10.0f, 10.0f);
        glTranslatef(1.0f, 0.5f, 8.0f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        if (tile == i) {
            glScalef(1.6f, 1.6f, 1.6f);
        }

        glTranslatef(-1.5f, 0.5f, 0.5f);
        glScalef(-1.0f, -1.0f, -1.0f);
        t->begin();
        tile1->render(t, minecraft->level, 0, -2, 0, 0);
        t->end();
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

void InventoryScreen::mousePressed(int32_t x, int32_t y, int32_t clickType) {
    if (clickType == 0) {
        shared_ptr<Minecraft> minecraft = this->minecraft.lock();
        if (!minecraft) {
            return;
        }

        shared_ptr<Inventory> inventory = minecraft->player->inventory;
        int32_t tile = getTileAtSlot(x, y);
        if (tile >= 0) {
            inventory->getSlotContainsTile(User::getCreativeTiles()[tile]);
        }

        minecraft->setScreen(nullptr);
    }
}