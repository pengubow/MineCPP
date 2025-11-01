#include <GLFW/glfw3.h>
#include "player/MovementInputFromOptions.h"
#include "Util/Util.h"
#include <iostream>
MovementInputFromOptions::MovementInputFromOptions(Options* options) 
    : options(options) {}

void MovementInputFromOptions::setKey(int32_t key, bool state) {
    uint8_t key1 = -1;

    if (key == options->forward.key) {
        key1 = 0;
    }

    if (key == options->back.key) {
        key1 = 1;
    }

    if (key == options->left.key) {
        key1 = 2;
    }

    if (key == options->right.key) {
        key1 = 3;
    }

    if (key == options->jump.key) {
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