#include "Player.h"
#include "Util.h"

Player::Player(shared_ptr<Level>& level) : Entity(level) {
    this->heightOffset = 1.62f;
}

void Player::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    float xa = 0.0f;
    float ya = 0.0f;
    if (Util::isKeyDown(GLFW_KEY_R)) {
        this->resetPos();
    }
    if (Util::isKeyDown(GLFW_KEY_UP) || Util::isKeyDown(GLFW_KEY_W)) {
        ya -= 1.0f;
    }
    if (Util::isKeyDown(GLFW_KEY_DOWN) || Util::isKeyDown(GLFW_KEY_S)) {
        ya += 1.0f;
    }
    if (Util::isKeyDown(GLFW_KEY_LEFT) || Util::isKeyDown(GLFW_KEY_A)) {
        xa -= 1.0f;
    }
    if (Util::isKeyDown(GLFW_KEY_RIGHT) || Util::isKeyDown(GLFW_KEY_D)) {
        xa += 1.0f;
    }
    if (Util::isKeyDown(GLFW_KEY_SPACE) && this->onGround) {
        this->yd = 0.5f;
    }
    this->moveRelative(xa, ya, this->onGround ? 0.1f : 0.02f);
    this->yd = (float)((double)this->yd - 0.08);
    this->move(this->xd, this->yd, this->zd);
    this->xd *= 0.91f;
    this->yd *= 0.98f;
    this->zd *= 0.91f;
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
    }
}