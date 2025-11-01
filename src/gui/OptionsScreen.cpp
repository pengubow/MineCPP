#include "gui/OptionsScreen.h"
#include "gui/ControlsScreen.h"
#include "Minecraft.h"

OptionsScreen::OptionsScreen(shared_ptr<Screen> parent, Options* options) 
    : parent(parent), options(options) {}

void OptionsScreen::init() {
    for (int32_t i = 0; i < 5; i++) {
        buttons.push_back(make_shared<Button>(i, width / 2 - 100, height / 6 + i * 24, options->getOption(i)));
    }

    buttons.push_back(make_shared<Button>(10, width / 2 - 100, height / 6 + 120 + 12, "Controls..."));
    buttons.push_back(make_shared<Button>(20, width / 2 - 100, height / 6 + 168, "Done"));
}

void OptionsScreen::buttonClicked(shared_ptr<Button>& button) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (button->enabled) {
        if (button->id < 5) {
            options->setOption(button->id, 1);
            button->msg = options->getOption(button->id);
        }

        if (button->id == 10) {
            minecraft->setScreen(make_shared<ControlsScreen>(shared_from_this(), options));
        }

        if (button->id == 20) {
            minecraft->setScreen(parent);
        }
    }
}

void OptionsScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
	drawCenteredString(font, title, width / 2, 20, 16777215);
	Screen::render(var1, var2);
}