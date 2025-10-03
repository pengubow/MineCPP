#pragma once

#include <GLFW/glfw3.h>
#include <atomic>
#include "level/Level.h"
#include "level/LevelRenderer.h"
#include "Player.h"
#include "HitResult.h"
#include "Timer.h"
#include "character/Zombie.h"
#include "gui/Font.h"
#include "MinecraftApplet.h"

class Minecraft {
public:
    string VERSION_STRING = "0.0.11a";
private:
    bool fullscreen = false;
    int32_t width;
    int32_t height;
    float fogColor0[4];
    float fogColor1[4];
    Timer timer = Timer(20.0f);
    shared_ptr<Level> level;
    shared_ptr<LevelRenderer> levelRenderer;
    shared_ptr<Player> player;
    int32_t paintTexture = 1;
    shared_ptr<ParticleEngine> particleEngine;
    vector<shared_ptr<Entity>> entities;
public:
    MinecraftApplet* applet;
    std::atomic<bool> pause = false;
private:
    int32_t yMouseAxis = 1;
public:
    shared_ptr<Textures> textures;
private:
    unique_ptr<Font> font;
    int32_t editMode = 0;
    std::atomic<bool> running = false;
    string fpsString = "";
    bool mouseGrabbed = false;
    vector<GLuint> selectBuffer = vector<GLuint>(2000);
    vector<GLint> viewportBuffer = vector<GLint>(16);
    unique_ptr<HitResult> hitResult;
    GLFWwindow* window;
    float lastMouseX, lastMouseY;
    float mouseDX, mouseDY;
    vector<float> lb;
public:
    Minecraft(int32_t width, int32_t height, bool fullscreen);

    void init();
private:
    void checkGlError(string string);
public:
    void destroy();
    void run();
    void stop();
    void grabMouse();
    void releaseMouse();
private:
    void handleMouseClick();
public:
    void tick();
private:
    bool isFree(shared_ptr<AABB>& aabb);
    void moveCameraToPlayer(float a);
    void setupCamera(float a);
    void setupPickCamera(float a, int32_t x, int32_t y);
    void pick(float a);
public:
    void render(float a);
private:
    void drawGui(float a);
    void setupFog(int32_t i);
    vector<float>& getBuffer(float a, float b, float c, float d);
    static void checkError();
};