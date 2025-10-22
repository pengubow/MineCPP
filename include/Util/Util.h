#pragma once

#include <GLFW/glfw3.h>
#include <random>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <thread>
#include <chrono>

using namespace std;

namespace Util {
    static GLFWwindow* win;
    static vector<int32_t> prevKeyState = vector<int32_t>(GLFW_KEY_LAST + 1);
    static vector<int32_t> prevMouseKeyState = vector<int32_t>(GLFW_MOUSE_BUTTON_8 + 1);
    static uint64_t multiplier = 25214903917;
    static uint64_t addend = 11;
    static uint64_t mask = 281474976710655;
    static uint64_t seed = ((uint64_t)(chrono::high_resolution_clock::now().time_since_epoch().count()) ^ multiplier) & mask;

    GLFWwindow* getGLFWWindow();
    bool isKeyDown(int32_t key);
    bool isKeyDownPrev(int32_t key);
    bool isMouseKeyDown(int32_t key);
    bool isMouseKeyDownPrev(int32_t key);
    int32_t next(int32_t bits);
    void setSeed(uint64_t seed);
    int32_t nextInt();
    int32_t nextInt(int32_t max);
    int32_t nextInt(int32_t min, int32_t max);
    float nextFloat();
    float nextFloat(float max);
    float nextFloat(float min, float max);
    double random();
    double random(double max);
    double random(double min, double max);
    string trim(const string& s);
}