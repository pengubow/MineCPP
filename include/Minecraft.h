#pragma once

#include <GLFW/glfw3.h>
#include <atomic>
#include "level/Level.h"
#include "renderer/LevelRenderer.h"
#include "player/Player.h"
#include "HitResult.h"
#include "Timer.h"
#include "character/Zombie.h"
#include "gui/Font.h"
#include "User.h"
#include "level/LevelIO.h"
#include "level/levelgen/LevelGen.h"
#include "MinecraftApplet.h"
#include "gui/Screen.h"
#include "net/ConnectionManager.h"

class Minecraft : public enable_shared_from_this<Minecraft> {
private:
    bool fullscreen = false;
public:
	int32_t width;
	int32_t height;
private:
	array<float, 4> fogColor0;
	array<float, 4> fogColor1;
	Timer timer = Timer(20.0f);
public:
	shared_ptr<Level> level;
	shared_ptr<LevelRenderer> levelRenderer;
	shared_ptr<Player> player;
private:
	int32_t paintTexture = 1;
	shared_ptr<ParticleEngine> particleEngine;
public:
	shared_ptr<User> user;
	string minecraftUri;
	atomic<bool> pause = false;
private:
	int32_t yMouseAxis = 1;
	static shared_ptr<Textures> textures;
public:
	shared_ptr<Font> font;
private:
	int32_t editMode = 0;
	shared_ptr<Screen> screen;
	shared_ptr<LevelIO> levelIo;
	shared_ptr<LevelGen> levelGen;
	int32_t ticksRan = 0;
public:
	string loadMapUser;
	int32_t loadMapID = 0;
private:
	ConnectionManager* sendQueue;
	static vector<int32_t> creativeTiles;
	float fogColorRed = 0.5f;
	float fogColorGreen = 0.8f;
	float fogColorBlue = 1.0f;
	atomic<bool> running = false;
	string fpsString = "";
	bool mouseGrabbed = false;
	int32_t prevFrameTime = 0;
	float renderDistance = 0.0f;
	vector<int32_t> viewportBuffer = vector<int32_t>(16);
	vector<GLuint> selectBuffer = vector<GLuint>(2000);
	optional<HitResult> hitResult;
	float fogColorMultiplier = 1.0f;
	atomic<int32_t> unusedInt1 = 0;
    atomic<int32_t> unusedInt2 = 0;
	vector<float> lb = vector<float>(16);
	string title = "";
	string text = "";
    static GLFWwindow* window;
public:
    Minecraft(int32_t width, int32_t height, bool fullscreen);

	void setServer(string var1, int32_t port);
    void setScreen(shared_ptr<Screen> screen);
private:
    void checkGlError(string string);
public:
    void destroy();
    void run();
    void stop();
    void grabMouse();
    void releaseMouse();
private:
    void clickMouse();
public:
    void tick();
private:
	bool isMultiplayer();
    void orientCamera(float a);
public:
    void render(float a);
private:
    void setupFog(int32_t i);
    vector<float>& getBuffer(float a, float b, float c, float d);
public:
    void beginLevelLoading(string title);
    void levelLoadUpdate(string text);
    void setLoadingProgress(int32_t var1);
    void generateLevel(int32_t var1);
	bool saveLevel(int32_t var1, string var2);
	bool loadLevel(string var1, int32_t var2);
    void setLevel(shared_ptr<Level> level);
};