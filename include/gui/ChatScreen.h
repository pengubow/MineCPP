#pragma once

#include "gui/Screen.h"

class ChatScreen : public Screen {
    string TypedMsg = "";
    int32_t counter = 0;
public:
    void init() override;
    void closeScreen() override;
    void tick() override;
protected:
    void keyPressed(char var1, int32_t key) override;
public:
    void render(int32_t var1, int32_t var2) override;
};