#pragma once

#include <memory>
#include "level/Level.h"
#include "phys/AABB.h"

class Entity {
protected:
    shared_ptr<Level> level;
public:
    float xo;
    float yo;
    float zo;
    float x;
    float y;
    float z;
    float xd;
    float yd;
    float zd;
    float yRot;
    float xRot;
    AABB bb;
    bool onGround = false;
    bool removed = false;
protected:
    float heightOffset = 0.0f;
    float bbWidth = 0.6f;
    float bbHeight = 1.8f;
public:
    Entity(shared_ptr<Level>& level);

    void remove();
    void resetPos();
protected:
    void setSize(float w, float h);
    void setPos(float x, float y, float z);
public:
    void turn(float xo, float yo);
    virtual void tick();
    void move(float xa, float ya, float za);
    void moveRelative(float xa, float za, float speed);
    bool isLit();
    virtual void render(float a);
};