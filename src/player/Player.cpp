#include "player/Player.h"
#include "Util/Util.h"

Player::Player(shared_ptr<Level>& level, shared_ptr<MovementInputFromOptions>& input) : Entity(level) {
    this->heightOffset = 1.62f;
    this->input = input;
}

void Player::tick() {
    Entity::tick();
    bool isInWater = this->isInWater();
    bool isInLava = this->isInLava();
    input->updatePlayerMoveState();
    if (input->jumpHeld) {
        if (isInWater) {
            yd += 0.04f;
        }
        else if (isInLava) {
            yd += 0.04f;
        }
        else if (onGround && !input->jump) {
            yd = 0.42f;
            input->jump = true;
        }
    }
    else {
        input->jump = false;
    }

    float var3;
    if (isInWater) {
        var3 = y;
        moveRelative(input->moveStrafe, input->moveForward, 0.02f);
        move(xd, yd, zd);
        xd *= 0.8f;
        yd *= 0.8f;
        zd *= 0.8f;
        yd = (float)((double)yd - 0.02);
        if (horizontalCollision && isFree(xd, yd + 0.6f - y + var3, zd)) {
            yd = 0.3f;
        }
    }
    else if (isInLava) {
        var3 = y;
        moveRelative(input->moveStrafe, input->moveForward, 0.02f);
        move(xd, yd, zd);
        xd *= 0.5f;
        yd *= 0.5f;
        zd *= 0.5f;
        yd = (float)((double)yd - 0.02);
        if (horizontalCollision && isFree(xd, yd + 0.6f - y + var3, zd)) {
            yd = 0.3f;
        }
    }
    else {
        moveRelative(input->moveStrafe, input->moveForward, onGround ? 0.1f : 0.02f);
        move(xd, yd, zd);
        xd *= 0.91f;
        yd *= 0.98f;
        zd *= 0.91f;
        yd = (float)((double)yd - 0.08);
        if (onGround) {
            xd *= 0.6f;
            zd *= 0.6f;
        }
    }
}

void Player::releaseAllKeys() {
    input->releaseAllKeys();
}

void Player::setKey(int32_t key, bool state) {
    input->setKey(key, state);
}