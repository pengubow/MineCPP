#pragma once

#include <GLFW/glfw3.h>
#include <random>
#include <cstdint>
#include <limits>

using namespace std;

namespace Util {
    static GLFWwindow* win;
    static vector<int32_t> prevKeyState = vector<int32_t>(GLFW_KEY_LAST + 1);
    static vector<int32_t> prevMouseKeyState = vector<int32_t>(GLFW_MOUSE_BUTTON_8 + 1);

    GLFWwindow* getGLFWWindow();
    bool isKeyDown(int32_t key);
    bool isKeyDownPrev(int32_t key);
    bool isMouseKeyDownPrev(int32_t key);
    mt19937& rng();
    void setSeed(uint32_t seed);
    float nextFloat(float min = 0.0f, float max = 1.0f);
    double random(double min = 0.0, double max = 1.0);
    int32_t nextInt(int32_t max = INT32_MAX);
    int32_t nextInt(int32_t min, int32_t max);
}