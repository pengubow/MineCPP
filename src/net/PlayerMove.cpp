#include "net/PlayerMove.h"

PlayerMove::PlayerMove(float x, float y, float z, float yRot, float xRot) 
    : x(x), y(y), z(z),
    yRot(yRot), xRot(xRot),
    rotating(true), moving(true) {}


PlayerMove::PlayerMove(float x, float y, float z) 
    : x(x), y(y), z(z),
    moving(true), rotating(false) {}

PlayerMove::PlayerMove(float yRot, float xRot) 
    : yRot(yRot), xRot(xRot),
    rotating(true), moving(false) {}