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

void Screen::keyPressed(char var1, int32_t key) {
    if (key == GLFW_KEY_ESCAPE) {
        shared_ptr<Minecraft> minecraft = this->minecraft.lock();
        if (!minecraft) {
            return;
        }
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

void Screen::fillGradient(int32_t var0, int32_t var1, int32_t x, int32_t y, int32_t var4, int32_t var5) {
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
    glVertex2f((float)x, 0.0F);
    glVertex2f(0.0F, 0.0F);
    glColor4f(var8, var9, var13, var7);
    glVertex2f(0.0F, (float)y);
    glVertex2f((float)x, (float)y);
    glEnd();
    glDisable(GL_BLEND);
}

void Screen::drawCenteredString(string msg, int32_t x, int32_t y, int32_t var4) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    shared_ptr<Font> font = minecraft->font;
    font->drawShadow(msg, x - font->width(msg) / 2, y, var4);
}

void Screen::drawString(string var1, int32_t var2, int32_t var3, int32_t var4) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    shared_ptr<Font> font = minecraft->font;
    font->drawShadow(var1, var2, var3, var4);
}

char getCharFromKey(int key, bool shift) {
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        char base = 'a' + (key - GLFW_KEY_A);
        return shift ? toupper(base) : base;
    }

    if (!shift) {
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            return '0' + (key - GLFW_KEY_0);
        }
    } else {
        switch (key) {
            case GLFW_KEY_0: return ')';
            case GLFW_KEY_1: return '!';
            case GLFW_KEY_2: return '@';
            case GLFW_KEY_3: return '#';
            case GLFW_KEY_4: return '$';
            case GLFW_KEY_5: return '%';
            case GLFW_KEY_6: return '^';
            case GLFW_KEY_7: return '&';
            case GLFW_KEY_8: return '*';
            case GLFW_KEY_9: return '(';
        }
    }

    switch (key) {
        case GLFW_KEY_SPACE: return ' ';
        case GLFW_KEY_COMMA: return shift ? '<' : ',';
        case GLFW_KEY_PERIOD: return shift ? '>' : '.';
        case GLFW_KEY_SLASH: return shift ? '?' : '/';
        case GLFW_KEY_SEMICOLON: return shift ? ':' : ';';
        case GLFW_KEY_APOSTROPHE: return shift ? '"' : '\'';
        case GLFW_KEY_LEFT_BRACKET: return shift ? '{' : '[';
        case GLFW_KEY_RIGHT_BRACKET: return shift ? '}' : ']';
        case GLFW_KEY_BACKSLASH: return shift ? '|' : '\\';
        case GLFW_KEY_MINUS: return shift ? '_' : '-';
        case GLFW_KEY_EQUAL: return shift ? '+' : '=';
        case GLFW_KEY_GRAVE_ACCENT: return shift ? '~' : '`';
    }
    
    return 0;
}

void Screen::updateEvents() {
    GLFWwindow* window = Util::getGLFWWindow();
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    vector<shared_ptr<Button>> buttonsSnapshot = this->buttons;

    while (true) {
        while (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT)) {
            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int32_t var1 = (int32_t)(mouseX * this->width / minecraft->width);
            int32_t var2 = (int32_t)(mouseY * this->height / minecraft->height);

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
                if (var5->enabled && var2 >= var5->x && var3 >= var5->y &&
                    var2 < var5->x + var5->w && var3 < var5->y + var5->h) {
                    this->buttonClicked(var5);
                }
                ++var4;
            }

            break;
        }

        for (int32_t key = 0; key <= GLFW_KEY_LAST; ++key) {
            while (Util::isKeyDownPrev(key)) {
                keyPressed(getCharFromKey(key, Util::isKeyDown(GLFW_MOD_SHIFT)), key);
            }
        }

        return;
    }
}

void Screen::tick() {}

void Screen::closeScreen() {}