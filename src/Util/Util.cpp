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

string Util::getKeyName(int32_t key) {
    switch(key) {
        case GLFW_KEY_SPACE: return "SPACE";
        case GLFW_KEY_ENTER: return "ENTER";
        case GLFW_KEY_TAB: return "TAB";
        case GLFW_KEY_ESCAPE: return "ESCAPE";
        case GLFW_KEY_BACKSPACE: return "BACKSPACE";
        case GLFW_KEY_DELETE: return "DELETE";
        case GLFW_KEY_INSERT: return "INSERT";
        case GLFW_KEY_UP: return "UP";
        case GLFW_KEY_DOWN: return "DOWN";
        case GLFW_KEY_LEFT: return "LEFT";
        case GLFW_KEY_RIGHT: return "RIGHT";
        case GLFW_KEY_PAGE_UP: return "PAGE_UP";
        case GLFW_KEY_PAGE_DOWN: return "PAGE_DOWN";
        case GLFW_KEY_HOME: return "HOME";
        case GLFW_KEY_END: return "END";
        case GLFW_KEY_F1: return "F1";
        case GLFW_KEY_F2: return "F2";
        case GLFW_KEY_F3: return "F3";
        case GLFW_KEY_F4: return "F4";
        case GLFW_KEY_F5: return "F5";
        case GLFW_KEY_F6: return "F6";
        case GLFW_KEY_F7: return "F7";
        case GLFW_KEY_F8: return "F8";
        case GLFW_KEY_F9: return "F9";
        case GLFW_KEY_F10: return "F10";
        case GLFW_KEY_F11: return "F11";
        case GLFW_KEY_F12: return "F12";
        case GLFW_KEY_LEFT_SHIFT: return "LEFT_SHIFT";
        case GLFW_KEY_RIGHT_SHIFT: return "RIGHT_SHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LEFT_CONTROL";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
        case GLFW_KEY_LEFT_ALT: return "LEFT_ALT";
        case GLFW_KEY_RIGHT_ALT: return "RIGHT_ALT";
        default: {
            const char* name = glfwGetKeyName(key, 0);
            if (name) {
                string result(name);
                for (auto& c : result) {
                    c = toupper(c);
                }
                return result;
            }
            return "UNKNOWN";
        }
    }
}