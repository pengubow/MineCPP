#include "gui/Screen.h"
#include "Minecraft.h"

void Screen::render(int32_t var1, int32_t var2) {
    for (int32_t var3 = 0; var3 < buttons.size(); ++var3) {
        shared_ptr<Button> var4 = buttons[var3];
        if (var4->visible) {
            if (!var4->enabled) {
                fill(var4->x - 1, var4->y - 1, var4->x + var4->w + 1, var4->y + var4->h + 1, -8355680);
                fill(var4->x, var4->y, var4->x + var4->w, var4->y + var4->h, -7303024);
                drawCenteredString(var4->msg, var4->x + var4->w / 2, var4->y + (var4->h - 8) / 2, -6250336);
            }
            else {
                fill(var4->x - 1, var4->y - 1, var4->x + var4->w + 1, var4->y + var4->h + 1, -16777216);
                if (var1 >= var4->x && var2 >= var4->y && var1 < var4->x + var4->w && var2 < var4->y + var4->h) {
                    fill(var4->x - 1, var4->y - 1, var4->x + var4->w + 1, var4->y + var4->h + 1, -6250336);
                    fill(var4->x, var4->y, var4->x + var4->w, var4->y + var4->h, -8355680);
                    drawCenteredString(var4->msg, var4->x + var4->w / 2, var4->y + (var4->h - 8) / 2, 16777120);
                }
                else {
                    fill(var4->x, var4->y, var4->x + var4->w, var4->y + var4->h, -9408400);
                    drawCenteredString(var4->msg, var4->x + var4->w / 2, var4->y + (var4->h - 8) / 2, 14737632);
                }
            }
        }
    }

}

void Screen::keyPressed(int32_t key) {
    if (key == GLFW_KEY_ESCAPE) {
        minecraft->setScreen(nullptr);
        minecraft->grabMouse();
    }

}

void Screen::buttonClicked(shared_ptr<Button>& var1) {}

void Screen::init(shared_ptr<Minecraft>& minecraft, int32_t width, int32_t height) {
    this->minecraft = minecraft;
    this->width = width;
    this->height = height;
    this->init();
}

void Screen::init() {}

void Screen::fill(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4) {
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

void Screen::fillGradient(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5) {
    float var9 = (float)96 / 255.0F;
    float var10 = (float)5 / 255.0F;
    float var11 = (float)5 / 255.0F;
    float var12 = (float)160 / 255.0F;
    float var6 = (float)48 / 255.0F;
    float var7 = (float)48 / 255.0F;
    float var8 = (float)96 / 255.0F;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(var10, var11, 0.0F, var9);
    glVertex2f((float)var2, 0.0F);
    glVertex2f(0.0F, 0.0F);
    glColor4f(var6, var7, var8, var12);
    glVertex2f(0.0F, (float)var3);
    glVertex2f((float)var2, (float)var3);
    glEnd();
    glDisable(GL_BLEND);
}

void Screen::drawCenteredString(string var1, int32_t var2, int32_t var3, int32_t var4) {
    shared_ptr<Font> font = minecraft->font;
    font->drawShadow(var1, var2 - font->width(var1) / 2, var3, var4);
}

void Screen::drawString(string var1, int32_t var2, int32_t var3, int32_t var4) {
    shared_ptr<Font> font = minecraft->font;
    font->drawShadow(var1, var2, var3, var4);
}

void Screen::updateEvents() {
    GLFWwindow* window = Util::getGLFWWindow();

    vector<shared_ptr<Button>> buttonsSnapshot = this->buttons;

    while (true) {
        while (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT)) {
            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int32_t var1 = (int32_t)(mouseX * this->width / this->minecraft->width);
            int32_t var2 = (int32_t)(mouseY * this->height / this->minecraft->height);

            int32_t var4 = 0;
            int32_t var3 = var2;
            var2 = var1;

            if (var4 != 0) {
                continue;
            }

            var4 = 0;
            while (var4 < buttonsSnapshot.size()) {
                shared_ptr<Button> var5 = buttonsSnapshot[var4];
                
                if (!var5) {
                    var4++;
                    continue;
                }
                if (var2 >= var5->x && var3 >= var5->y &&
                    var2 < var5->x + var5->w && var3 < var5->y + var5->h) {
                    this->buttonClicked(var5);
                }
                ++var4;
            }

            break;
        }

        for (int32_t key = 0; key <= GLFW_KEY_LAST; ++key) {
            while (Util::isKeyDownPrev(key)) {
                keyPressed(key);
            }
        }

        return;
    }
}

void Screen::tick() {}

void Screen::closeScreen() {}