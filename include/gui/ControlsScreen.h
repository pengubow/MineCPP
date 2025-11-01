#pragma once

#include "gui/Screen.h"
#include "Options.h"

class ControlsScreen : public Screen {
private:
    shared_ptr<Screen> parent;
    string title = "controls";
    Options* options;
    int32_t selectedKey = -1;
public:
    ControlsScreen(shared_ptr<Screen> parent, Options* options);

    void init();
protected:
    void buttonClicked(shared_ptr<Button>& button) override;
    void keyPressed(char var1, int32_t key) override;
public:
    void render(int32_t var1, int32_t var2) override;
};