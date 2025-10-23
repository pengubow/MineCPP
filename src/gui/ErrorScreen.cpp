#include "gui/ErrorScreen.h"

ErrorScreen::ErrorScreen(string title, string desc) 
    : title(title), desc(desc) {}

void ErrorScreen::init() {}

void ErrorScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, -12574688, -11530224);
    drawCenteredString(title, width / 2, 90, 16777215);
    drawCenteredString(desc, width / 2, 110, 16777215);
    Screen::render(var1, var2);
}

void ErrorScreen::keyPressed(char var1, int32_t key) {}