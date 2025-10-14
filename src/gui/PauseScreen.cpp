#include "gui/PauseScreen.h"
#include "gui/NewLevelScreen.h"
#include "gui/SaveLevelScreen.h"
#include "gui/LoadLevelScreen.h"
#include "Minecraft.h"

void PauseScreen::init() {
    buttons.clear();
    buttons.push_back(make_shared<Button>(0, width / 2 - 100, height / 3, 200, 20, "Generate new level..."));
    buttons.push_back(make_shared<Button>(1, width / 2 - 100, height / 3 + 32, 200, 20, "Save level.."));
    buttons.push_back(make_shared<Button>(2, width / 2 - 100, height / 3 + 64, 200, 20, "Load level.."));
    buttons.push_back(make_shared<Button>(3, width / 2 - 100, height / 3 + 96, 200, 20, "Back to game"));
    if (minecraft->user == nullptr) {
        buttons[1]->enabled = false;
        buttons[2]->enabled = false;
    }

}

void PauseScreen::buttonClicked(shared_ptr<Button>& button) {
    if(button->id == 0) {
        minecraft->setScreen(make_shared<NewLevelScreen>(shared_from_this()));
    }

    if(minecraft->user != nullptr) {
        if(button->id == 1) {
            minecraft->setScreen(make_shared<SaveLevelScreen>(shared_from_this()));
        }

        if(button->id == 2) {
            minecraft->setScreen(make_shared<LoadLevelScreen>(shared_from_this()));
        }
    }

    if(button->id == 3) {
        minecraft->setScreen(nullptr);
        minecraft->grabMouse();
    }

}

void PauseScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString("Game menu", width / 2, 40, 16777215);
    Screen::render(var1, var2);
}