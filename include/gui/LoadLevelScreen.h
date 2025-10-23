#pragma once

#include "gui/Screen.h"

class LoadLevelScreen : public Screen {
    shared_ptr<Screen> parent;
    bool finished = false;
    bool loaded = false;
    vector<string> levels;
    string status = "";
protected:
    string title = "Load level";
public:
    LoadLevelScreen(shared_ptr<Screen> parent);
    void run();
protected:
    virtual void setLevels(vector<string>& var1);
public:
    void init() override;
protected:
    void buttonClicked(shared_ptr<Button>& button) override;
    virtual void loadLevel(int32_t var1);
public:
    void render(int32_t var1, int32_t var2) override;
};