#pragma once

class PlayerMove {
public:
    float x;
    float y;
    float z;
    float yRot;
    float xRot;
    bool rotating = false;
    bool moving = false;

    PlayerMove(float x, float y, float z, float yRot, float xRot);
    PlayerMove(float x, float y, float z);
    PlayerMove(float yRot, float xRot);
};