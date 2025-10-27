#include "Util/Util.h"
#include <cctype>

static vector<int32_t> prevKeyState(GLFW_KEY_LAST + 1, GLFW_RELEASE);
static vector<int32_t> prevMouseKeyState(GLFW_MOUSE_BUTTON_8 + 1, GLFW_RELEASE);

GLFWwindow* Util::getGLFWWindow() {
    if (!win) {
        win = glfwGetCurrentContext();
    }
    return win;
}

bool Util::windowIsActive() {
    return glfwGetWindowAttrib(getGLFWWindow(), GLFW_FOCUSED);
}

bool Util::isKeyDown(int32_t key) {
    win = getGLFWWindow();
    if (!win) 
        return false;
    return glfwGetKey(win, key) == GLFW_PRESS;
}

bool Util::isKeyDownPrev(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    
    int keyState = glfwGetKey(win, key);
    bool justPressed = (keyState == GLFW_PRESS && prevKeyState[key] == GLFW_RELEASE);
    prevKeyState[key] = keyState;
    return justPressed;
}

bool Util::isMouseKeyDown(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    return glfwGetMouseButton(win, key) == GLFW_PRESS;
}

bool Util::isMouseKeyDownPrev(int32_t key) {
    GLFWwindow* win = getGLFWWindow();
    if (!win) 
        return false;
    
    int keyState = glfwGetMouseButton(win, key);
    bool justPressed = (keyState == GLFW_PRESS && prevMouseKeyState[key] == GLFW_RELEASE);
    prevMouseKeyState[key] = keyState;
    return justPressed;
}

string Util::trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    if (start == s.size()) return "";

    size_t end = s.size() - 1;
    while (end > start && isspace(static_cast<unsigned char>(s[end]))) {
        end--;
    }

    return s.substr(start, end - start + 1);
}