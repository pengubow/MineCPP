#pragma once

#include <memory>
#include "level/Level.h"
#include "phys/AABB.h"
#include "renderer/Textures.h"

class Entity {
public:
    static const int64_t serialVersionUID = 0;
    weak_ptr<Level> level;
public:
    float xo = 0.0f;
    float yo = 0.0f;
    float zo = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;
    float yRot = 0.0f;
    float xRot = 0.0f;
    float yRotI = 0.0f;
    float xRotI = 0.0f;
    AABB bb;
    bool onGround = false;
    bool horizontalCollision = false;
    bool removed = false;
    float heightOffset = 0.0f;
    float bbWidth = 0.6f;
    float bbHeight = 1.8f;

    Entity(shared_ptr<Level>& level);

    void resetPos();
public:
    void remove();
protected:
    void setSize(float w, float h);
    void setPos(float x, float y, float z);
public:
    void turn(float xo, float yo);
    void interpolateTurn(float xo, float yo);
    virtual void tick();
    bool isFree(float x, float y, float z);
    void move(float xa, float ya, float za);
    bool isInWater();
    bool isInLava();
    void moveRelative(float xa, float za, float speed);
    bool isLit();
    float getBrightness();
    virtual void render(shared_ptr<Textures>& textures, float a);
    void setLevel(shared_ptr<Level>& level);
    void moveTo(float x, float y, float z, float xRot, float yRot);
};