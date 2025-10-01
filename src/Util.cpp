#include "Util.h"

GLFWwindow* Util::getGLFWWindow() {
    if (!win) {
        win = glfwGetCurrentContext();
    }
    return win;
}

bool Util::isKeyDown(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    return glfwGetKey(win, key) == GLFW_PRESS;
}

bool Util::isKeyDownPrev(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    int keyState = glfwGetKey(win, key);
    bool justPressed = keyState == GLFW_PRESS && prevKeyState[key] == GLFW_RELEASE;
    prevKeyState[key] = keyState;
    return justPressed;
}

bool Util::isMouseKeyDownPrev(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    int keyState = glfwGetMouseButton(win, key);
    bool justPressed = keyState == GLFW_PRESS && prevMouseKeyState[key] == GLFW_RELEASE;
    prevMouseKeyState[key] = keyState;
    return justPressed;
}

std::mt19937& Util::rng() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

void Util::setSeed(uint32_t seed) {
    rng().seed(seed);
}

float Util::randomfr(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng());
}

int32_t Util::nextInt(int32_t max) {
    return nextInt(0, max);
}

int32_t Util::nextInt(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> dist(min, max - 1);
    return dist(rng());
}