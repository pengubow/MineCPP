#include "gui/NewLevelScreen.h"
#include "Minecraft.h"

NewLevelScreen::NewLevelScreen(shared_ptr<Screen> parent)
    : parent(parent) {}

void NewLevelScreen::init() {
    buttons.clear();
	buttons.push_back(make_shared<Button>(0, width / 2 - 100, height / 3, "Small"));
	buttons.push_back(make_shared<Button>(1, width / 2 - 100, height / 3 + 32, "Normal"));
	buttons.push_back(make_shared<Button>(2, width / 2 - 100, height / 3 + 64, "Huge"));
    buttons.push_back(make_shared<Button>(3, width / 2 - 100, height / 3 + 96, "Cancel"));
}

void NewLevelScreen::buttonClicked(shared_ptr<Button>& var1) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    if(var1->id == 3) {
        minecraft->setScreen(parent);
    } else {
        minecraft->generateLevel(var1->id);
        minecraft->setScreen(nullptr);
        minecraft->grabMouse();
    }
}

void NewLevelScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString(font, "Generate new level", width / 2, 40, 16777215);
    Screen::render(var1, var2);
}