#pragma once

#include <GLFW/glfw3.h>
#include <atomic>
#include <deque>
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
#include "ChatLine.h"
#include "sound/SoundPlayer.h"
#include "sound/SoundManager.h"
#include "renderer/RenderHelper.h"
#include "ProgressListener.h"
#include "BackgroundDownloader.h"
#include "Options.h"

class InGameHud;

class Minecraft : public enable_shared_from_this<Minecraft> {
    bool fullscreen = false;
public:
	int32_t width;
	int32_t height;
private:
	Timer timer = Timer(20.0f);
public:
	shared_ptr<Level> level;
	unique_ptr<LevelRenderer> levelRenderer;
	shared_ptr<Player> player;
	shared_ptr<ParticleEngine> particleEngine;
	shared_ptr<User> user;
	string minecraftUri;
	atomic<bool> pause = false;
	static shared_ptr<Textures> textures;
	shared_ptr<Font> font;
	int32_t editMode = 0;
	shared_ptr<Screen> screen;
	shared_ptr<ProgressListener> loadingScreen;
	unique_ptr<RenderHelper> renderHelper;
	unique_ptr<LevelIO> levelIo;
private:
	unique_ptr<LevelGen> levelGen;
public:
	unique_ptr<SoundManager> soundManager;
private:
	unique_ptr<BackgroundDownloader> backgroundDownloader;
	int32_t ticksRan = 0;
public:
	string loadMapUser;
	int32_t loadMapId = 0;
	shared_ptr<InGameHud> hud;
	bool hideGui = false;
	ZombieModel playerModel = ZombieModel();
	ConnectionManager* connectionManager = nullptr;
	shared_ptr<SoundPlayer> soundPlayer;
	optional<HitResult> hitResult;
	unique_ptr<Options> options;
	string server;
	int32_t port = 0;
	atomic<bool> running = false;
	string fpsString = "";
	bool mouseGrabbed = false;
private:
	int32_t prevFrameTime = 0;
	vector<int32_t> viewportBuffer = vector<int32_t>(16);
	vector<GLuint> selectBuffer = vector<GLuint>(2000);
	string title = "";
	string text = "";
public:
    static GLFWwindow* window;

    Minecraft(int32_t width, int32_t height, bool fullscreen);

    void setScreen(shared_ptr<Screen> screen);
private:
    void checkGlError(string string);
public:
    void destroy();
    void run();
    void grabMouse();
    void pauseGame();
private:
    void clickMouse();
public:
    void tick();
private:
    void render(float a);
	bool isMultiplayer();
public:
    void generateLevel(int32_t var1);
	bool loadLevel(string var1, int32_t var2);
    void setLevel(shared_ptr<Level> level);
};