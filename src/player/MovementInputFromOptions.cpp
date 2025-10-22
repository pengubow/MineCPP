#include <GLFW/glfw3.h>
#include "player/MovementInputFromOptions.h"
#include "Util/Util.h"

void MovementInputFromOptions::setKey(int32_t key, bool state) {
    uint8_t key1 = -1;
    if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
        key1 = 0;
    }

    if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
        key1 = 1;
    }

    if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) {
        key1 = 2;
    }

    if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) {
        key1 = 3;
    }

    if (key == GLFW_KEY_SPACE) {
        key1 = 4;
    }

    if (key1 >= 0 && key1 < 10) {
        keys[key1] = state;
    }
}

void MovementInputFromOptions::releaseAllKeys() {
    for (int32_t i = 0; i < 10; i++) {
        keys[i] = false;
    }
}

void MovementInputFromOptions::updatePlayerMoveState() {
    moveStrafe = 0.0f;
    moveForward = 0.0f;
    if (keys[0]) {
        moveForward--;
    }

    if (keys[1]) {
        moveForward++;
    }

    if (keys[2]) {
        moveStrafe--;
    }

    if (keys[3]) {
        moveStrafe++;
    }

    jumpHeld = keys[4];
}