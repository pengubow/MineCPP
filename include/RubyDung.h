#pragma once

#include <GLFW/glfw3.h>
#include "level/Level.h"
#include "level/LevelRenderer.h"
#include "Player.h"
#include "HitResult.h"
#include "Timer.h"
#include "character/Zombie.h"

class RubyDung {
private:
    static const bool FULLSCREEN_MODE = false;
    int32_t width = 1024;
    int32_t height = 768;
    float fogColor0[4];
    float fogColor1[4];
    Timer timer = 20.0f;
    std::shared_ptr<Level> level;
    std::shared_ptr<LevelRenderer> levelRenderer;
    std::shared_ptr<Player> player;
    int32_t paintTexture = 1;
    std::shared_ptr<ParticleEngine> particleEngine;
    std::vector<std::shared_ptr<Zombie>> zombies;
    std::vector<GLuint> selectBuffer = std::vector<GLuint>(2000);
    std::vector<GLint> viewportBuffer = std::vector<GLint>(16);
    std::unique_ptr<HitResult> hitResult;
    GLFWwindow* window;
    float lastMouseX, lastMouseY;
    float mouseDX, mouseDY;
    std::vector<float> lb;

public:
    RubyDung();

    void init();
    void destroy();
    void run();
    void tick();
private:
    void moveCameraToPlayer(float a);
    void setupCamera(float a);
    void setupPickCamera(float a, int32_t x, int32_t y);
    void pick(float a);
public:
    void render(float a);
private:
    void drawGui(float a);
    void setupFog(int32_t i);
    std::vector<float>& getBuffer(float a, float b, float c, float d);
    static void checkError();
};