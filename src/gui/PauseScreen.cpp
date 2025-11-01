#include "gui/PauseScreen.h"
#include "gui/NewLevelScreen.h"
#include "gui/SaveLevelScreen.h"
#include "gui/LoadLevelScreen.h"
#include "gui/OptionsScreen.h"
#include "Minecraft.h"

void PauseScreen::init() {
    buttons.clear();
    buttons.push_back(make_shared<Button>(0, width / 2 - 100, height / 3, "Options..."));
    buttons.push_back(make_shared<Button>(1, width / 2 - 100, height / 3 + 24, "Generate new level..."));
    buttons.push_back(make_shared<Button>(2, width / 2 - 100, height / 3 + 48, "Save level.."));
    buttons.push_back(make_shared<Button>(3, width / 2 - 100, height / 3 + 72, "Load level.."));
    buttons.push_back(make_shared<Button>(4, width / 2 - 100, height / 3 + 120, "Back to game"));
    
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (minecraft->user == nullptr) {
        buttons[2]->enabled = false;
        buttons[3]->enabled = false;
    }

    if (minecraft->connectionManager != nullptr) {
        buttons[1]->enabled = false;
        buttons[2]->enabled = false;
        buttons[3]->enabled = false;
    }

}

void PauseScreen::buttonClicked(shared_ptr<Button>& button) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    
    if (button->id == 0) {
        minecraft->setScreen(make_shared<OptionsScreen>(shared_from_this(), minecraft->options.get()));
    }

    if (button->id == 1) {
        minecraft->setScreen(make_shared<NewLevelScreen>(shared_from_this()));
    }

    if (minecraft->user != nullptr) {
        if (button->id == 2) {
            minecraft->setScreen(make_shared<SaveLevelScreen>(shared_from_this()));
        }

        if (button->id == 3) {
            minecraft->setScreen(make_shared<LoadLevelScreen>(shared_from_this()));
        }
    }

    if (button->id == 4) {
        minecraft->setScreen(nullptr);
        minecraft->grabMouse();
    }

}

void PauseScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString(font, "Game menu", width / 2, 40, 16777215);
    Screen::render(var1, var2);
}