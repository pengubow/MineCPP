#pragma once

#include <deque>
#include "Entity.h"
#include "net/PlayerMove.h"
#include "character/ZombieModel.h"

class Minecraft;

class NetworkPlayer : public Entity {
    ZombieModel zombieModel;
    float animStep = 0.0f;
    float animStepO = 0.0f;
    deque<PlayerMove> moveQueue;
public:
    weak_ptr<Minecraft> minecraft;
private:
    int32_t xp;
    int32_t yp;
    int32_t zp;
    float yBodyRot = 0.0f;
    float yBodyRotO = 0.0f;
    float oRun;
    float run = 0.0f;
    int32_t skin = -1;
public:
    int32_t skinWidth;
    int32_t skinHeight;
    uint8_t* newTexture = nullptr;
    string name;
    string displayName;
    int32_t tickCount = 0;
private:
    shared_ptr<Textures> textures;
public:
    NetworkPlayer(shared_ptr<Minecraft>& minecraft, int32_t id, string name, int32_t x, int32_t y, int32_t z, float yRot, float xRot);
    
    void tick() override;
    void render(shared_ptr<Textures>& textures, float a) override;
    
    void queue(int8_t dx, int8_t dy, int8_t dz, float yRot, float xRot);
    void teleport(int16_t x, int16_t y, int16_t z, float yRot, float xRot);
    void queue(int8_t dx, int8_t dy, int8_t dz);
    void queue(float yRot, float xRot);
    void clear();
};