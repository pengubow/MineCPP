#pragma once

class PlayerMove {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float yRot = 0.0f;
    float xRot = 0.0f;
    bool rotating = false;
    bool moving = false;

    PlayerMove(float x, float y, float z, float yRot, float xRot);
    PlayerMove(float x, float y, float z);
    PlayerMove(float yRot, float xRot);
};