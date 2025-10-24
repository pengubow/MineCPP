#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

namespace Util {
    static GLFWwindow* win;

    GLFWwindow* getGLFWWindow();
    bool isKeyDown(int32_t key);
    bool isKeyDownPrev(int32_t key);
    bool isMouseKeyDown(int32_t key);
    bool isMouseKeyDownPrev(int32_t key);
    string trim(const string& s);
}