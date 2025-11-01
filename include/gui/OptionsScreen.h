#pragma once

#include "gui/Screen.h"
#include "Options.h"

class OptionsScreen : public Screen, public enable_shared_from_this<OptionsScreen> {
    shared_ptr<Screen> parent;
    string title = "Options";
    Options* options;
public:
    OptionsScreen(shared_ptr<Screen> parent, Options* options);

    void init();
protected:
    void buttonClicked(shared_ptr<Button>& button) override;
public:
    void render(int32_t var1, int32_t var2) override;
};