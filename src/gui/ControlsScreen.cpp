#include "gui/ControlsScreen.h"
#include "gui/KeyBindingButton.h"
#include "Minecraft.h"

ControlsScreen::ControlsScreen(shared_ptr<Screen> parent, Options* options) 
    : parent(parent), options(options) {}

void ControlsScreen::init() {
    for (int32_t i = 0; i < options->keyBindings.size(); i++) {
        buttons.push_back(make_shared<KeyBindingButton>(i, width / 2 - 155 + i % 2 * 160, height / 6 + 24 * (i >> 1), options->getKeyBinding(i)));
    }

    buttons.push_back(make_shared<Button>(200, width / 2 - 100, height / 6 + 168, "Done"));
}

void ControlsScreen::buttonClicked(shared_ptr<Button>& button) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    for (int32_t i = 0; i < options->keyBindings.size(); i++) {
        buttons[i]->msg = options->getKeyBinding(i);
    }

    if (button->id) {
        minecraft->setScreen(parent);
    }
    else {
        selectedKey = button->id;
        button->msg = "> " + options->getKeyBinding(button->id) + " <";
    }
}

void ControlsScreen::keyPressed(char var1, int32_t key) {
    if (selectedKey >= 0) {
        options->setKeyBinding(selectedKey, key);
        buttons[selectedKey]->msg = options->getKeyBinding(selectedKey);
        selectedKey = -1;
    }
    else {
        Screen::keyPressed(var1, key);
    }
}

void ControlsScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString(font, title, width / 2, 20, 16777215);
	Screen::render(var1, var2);
}