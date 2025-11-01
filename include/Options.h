#pragma once

#include <vector>
#include <memory>
#include <filesystem>
#include <GLFW/glfw3.h>
#include "KeyBinding.h"

namespace fs = filesystem;

class Minecraft;

class Options {
    static const vector<string> RENDER_DISTANCES;
public:
    bool music = true;
    bool sound = true;
    bool invertMouse = false;
    bool showFPS = false;
    int32_t renderDistance = 0;
    KeyBinding forward = KeyBinding("Forward", GLFW_KEY_W);
    KeyBinding left = KeyBinding("Left", GLFW_KEY_A);
    KeyBinding back = KeyBinding("Back", GLFW_KEY_S);
    KeyBinding right = KeyBinding("Right", GLFW_KEY_D);
    KeyBinding jump = KeyBinding("Jump", GLFW_KEY_SPACE);
    KeyBinding build = KeyBinding("Build", GLFW_KEY_B);
    KeyBinding chat = KeyBinding("Chat", GLFW_KEY_T);
    KeyBinding toggleFog = KeyBinding("Toggle fog", GLFW_KEY_F);
    KeyBinding save = KeyBinding("Save location", GLFW_KEY_ENTER);
    KeyBinding load = KeyBinding("Load location", GLFW_KEY_R);
    vector<KeyBinding*> keyBindings = {&forward, &left, &back, &right, &jump, &build, &chat, &toggleFog, &save, &load};
private:
    fs::path optionsFile;
public:
    Options(shared_ptr<Minecraft>& minecraft, fs::path optionsDir);

    string getKeyBinding(int32_t binding);
    void setKeyBinding(int32_t binding, int32_t key);
    void setOption(int32_t var1, int32_t var2);
    string getOption(int32_t var1);
private:
    void loadOptions();
    void saveOptions();
};