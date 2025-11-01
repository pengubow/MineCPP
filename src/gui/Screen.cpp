#include "gui/Screen.h"
#include "Minecraft.h"

void Screen::render(int32_t var1, int32_t var2) {
    for (int32_t var3 = 0; var3 < buttons.size(); ++var3) {
        shared_ptr<Button> button = buttons[var3];
        shared_ptr<Minecraft> minecraft = this->minecraft.lock();
        if (!minecraft) {
            return;
        }

        if (button->visible) {
            shared_ptr<Font> font = minecraft->font;
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, minecraft->textures->getTextureId("resources/textures/gui.png"));
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            uint8_t var9 = 1;
            bool var6 = var1 >= button->w && var2 >= button->h && var1 < button->w + button->x && var2 < button->h + button->y;
            if(!button->enabled) {
                var9 = 0;
            } else if(var6) {
                var9 = 2;
            }

            button->blit(button->w, button->h, 0, 46 + var9 * 20, button->x / 2, button->y);
            button->blit(button->w + button->x / 2, button->h, 200 - button->x / 2, 46 + var9 * 20, button->x / 2, button->y);
            if(!button->enabled) {
                Button::drawCenteredString(font, button->msg, button->w + button->x / 2, button->h + (button->y - 8) / 2, -6250336);
            } else if(var6) {
                Button::drawCenteredString(font, button->msg, button->w + button->x / 2, button->h + (button->y - 8) / 2, 16777120);
            } else {
                Button::drawCenteredString(font, button->msg, button->w + button->x / 2, button->h + (button->y - 8) / 2, 14737632);
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

void Screen::mousePressed(int32_t x, int32_t y, int32_t clickType) {
    if (clickType == 0) {
        for (int32_t i = 0; i < buttons.size(); i++) {
            shared_ptr<Button> button = buttons[i];
            if (button->enabled && x >= button->w && y >= button->h && x < button->w + button->x && y < button->h + button->y) {
                buttonClicked(button);
            }
        }
    }
}

void Screen::buttonClicked(shared_ptr<Button>& button) {}

void Screen::init(shared_ptr<Minecraft>& minecraft, int32_t width, int32_t height) {
    this->minecraft = minecraft;
    font = minecraft->font;
    this->width = width;
    this->height = height;
    this->init();
}

void Screen::init() {}

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
    updateMouseEvents();
    updateKeyboardEvents();
}

void Screen::updateMouseEvents() {
    GLFWwindow* window = Util::getGLFWWindow();
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    vector<shared_ptr<Button>> buttonsSnapshot = this->buttons;

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int32_t var1 = (int32_t)(mouseX * this->width / minecraft->width);
    int32_t var2 = (int32_t)(mouseY * this->height / minecraft->height);

    int32_t clickType = -1;
    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT)) {
        clickType = 0;
    }

    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_RIGHT)) {
        clickType = 1;
    }

    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_MIDDLE)) {
        clickType = 2;
    }

    if (clickType != -1) {
        mousePressed(var1, var2, clickType);
    }
}

void Screen::updateKeyboardEvents() {
    for (int32_t key = 0; key <= GLFW_KEY_LAST; ++key) {
        if (Util::isKeyDownPrev(key)) {
            keyPressed(getCharFromKey(key, Util::isKeyDown(GLFW_MOD_SHIFT)), key);
        }
    }
}

void Screen::tick() {}

void Screen::closeScreen() {}