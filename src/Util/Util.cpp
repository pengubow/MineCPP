#include "Util/Util.h"

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
    bool justPressed = keyState == GLFW_PRESS && prevMouseKeyState[key] == GLFW_RELEASE;
    prevMouseKeyState[key] = keyState;
    return justPressed;
}

int32_t Util::next(int32_t bits) {
    seed = (seed * multiplier + addend) & mask;
    return (int32_t)(seed >> (48 - bits));
}

void Util::setSeed(uint64_t seed) {
    Util::seed = (seed ^ multiplier) & mask;
}

int32_t Util::nextInt() {
    return next(32);
}

int32_t Util::nextInt(int32_t max) {
    if (max <= 0) {
        throw invalid_argument("nextInt requires max > 0");
    }

    if ((max & (max - 1)) == 0) {
        int32_t r = next(31);
        return (int32_t)(((int64_t)r * max) >> 31);
    }

    int32_t bits, val;
    do {
        bits = next(31);
        val = bits % max;
    } while (bits - val + (max - 1) < 0);
    return val;
}

int32_t Util::nextInt(int32_t min, int32_t max) {
    if (max < min) {
        throw invalid_argument("nextInt requires max > min");
    }
    int32_t range = max - min;
    return min + nextInt(range);
}

float Util::nextFloat() {
    int32_t bits = next(24);
    return (float)bits / 16777216.0f;
}

float Util::nextFloat(float max) {
    if (max < 0.0f) {
        throw invalid_argument("nextFloat requires max > 0");
    }
    return nextFloat() * max;
}

float Util::nextFloat(float min, float max) {
    if (max < min) {
        throw invalid_argument("nextFloat requires max > min");
    }
    float r = nextFloat();
    return min + r * (max - min);
}

double Util::random() {
    uint64_t a = (uint64_t)next(26);
    uint64_t b = (uint64_t)next(27);
    uint64_t combined = (a << 27) + b;
    return (double)combined / (double)(9007199254740992);
}

double Util::random(double max) {
    if (max < 0.0) {
        throw invalid_argument("random requires max > 0");
    }
    return random() * max;
}

double Util::random(double min, double max) {
    if (max < min) {
        throw invalid_argument("random requires max > min");
    }
    return min + random() * (max - min);
}

string Util::trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;

    if (start == s.size()) return "";

    size_t end = s.size() - 1;
    while (end > start && isspace(static_cast<unsigned char>(s[end]))) end--;

    return s.substr(start, end - start + 1);
}

void Util::gzreadExact(gzFile file, void* buf, unsigned int len) {
    if (len == 0) {
        return;
    }

    int r = gzread(file, buf, len);
    if (r != (int)len) {
        throw runtime_error("gzread error");
    }
}

void Util::gzwriteExact(gzFile file, const void* buf, unsigned int len) {
    if (len == 0) {
        return;
    }

    int w = gzwrite(file, buf, len);
    if (w != (int)len) {
        throw runtime_error("gzwrite failed");
    }
}