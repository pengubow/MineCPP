#include "gui/SaveLevelScreen.h"
#include "gui/NameLevelScreen.h"
#include "Minecraft.h"

SaveLevelScreen::SaveLevelScreen(shared_ptr<Screen> parent) 
    : LoadLevelScreen(parent) {
    title = "Save Level";
}

void SaveLevelScreen::setLevels(vector<string>& var1) {
    for (int32_t var2 = 0; var2 < 5; ++var2) {
        buttons[var2]->msg = var1[var2];
        buttons[var2]->visible = true;
    }
}

void SaveLevelScreen::loadLevel(int32_t var1) {
    minecraft->setScreen(make_shared<NameLevelScreen>(shared_from_this(), buttons[var1]->msg, var1));
}