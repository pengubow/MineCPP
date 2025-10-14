#include <GLFW/glfw3.h>
#include "gui/NameLevelScreen.h"
#include "Util/Util.h"

NameLevelScreen::NameLevelScreen(shared_ptr<Screen> parent, string name, int32_t id) 
    : parent(parent), name(name), id(id) {
    if(this->name == "-") {
        this->name = "";
    }
}

void NameLevelScreen::init() {
    buttons.clear();
    glfwSetInputMode(Util::getGLFWWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);
    buttons.push_back(make_shared<Button>(0, width / 2 - 100, height / 4 + 120, 200, 20, "Save"));
    buttons.push_back(make_shared<Button>(1, width / 2 - 100, height / 4 + 144, 200, 20, "Cancel"));
    buttons[0]->enabled = Util::trim(name).length() > 1;
}

void NameLevelScreen::keyPressed(char var1, int32_t key) {
    if (key == GLFW_KEY_BACKSPACE && !name.empty()) {
        name.pop_back();
    }

    const std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,.:-_'*!\"#%/()=+?[]{}<>";
    if (allowed.find(var1) != std::string::npos) {
        name += var1;
    }

    buttons[0]->enabled = Util::trim(name).length() > 1;
}

void NameLevelScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString(title, width / 2, 40, 16777215);
    int32_t var3 = width / 2 - 100;
    int32_t var4 = height / 2 - 10;
    fill(var3 - 1, var4 - 1, var3 + 200 + 1, var4 + 20 + 1, -6250336);
    fill(var3, var4, var3 + 200, var4 + 20, -16777216);
    drawString(name + (counter / 6 % 2 == 0 ? "_" : ""), var3 + 4, var4 + 6, 14737632);
    Screen::render(var1, var2);
}