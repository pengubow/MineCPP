#pragma once

#include <GLFW/glfw3.h>
#include <random>
#include <cstdint>
#include <limits>

namespace Util {
    static GLFWwindow* win;
    static std::vector<int32_t> prevKeyState = std::vector<int32_t>(GLFW_KEY_LAST + 1);
    static std::vector<int32_t> prevMouseKeyState = std::vector<int32_t>(GLFW_MOUSE_BUTTON_8 + 1);

    GLFWwindow* getGLFWWindow();

    bool isKeyDown(int32_t key);
    bool isKeyDownPrev(int32_t key);
    bool isMouseKeyDownPrev(int32_t key);
    std::mt19937& rng();
    void setSeed(uint32_t seed);
    float randomfr(float min = 0.0f, float max = 1.0f);
    int32_t nextInt(int32_t max = INT32_MAX);
    int32_t nextInt(int32_t min, int32_t max);
}