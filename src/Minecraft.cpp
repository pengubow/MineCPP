#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <thread>
#include <algorithm>
#include <set>
#include "GL_compat.h"
#include "Util/tinyfiledialogs.h"
#include "Minecraft.h"
#include "gui/PauseScreen.h"
#include "gui/ErrorScreen.h"
#include "gui/ChatScreen.h"
#include "renderer/DirtyChunkSorter.h"
#include "comm/SocketConnection.h"
#include "net/Packet.h"
#include "net/NetworkPlayer.h"
#include "StopGameException.h"
#include "renderer/texture/TextureLavaFX.h"
#include "renderer/texture/TextureWaterFX.h"
#include "gui/InGameHud.h"
#include "gui/InventoryScreen.h"

static int32_t scrollWheel = 0;

void scroll_callback(GLFWwindow*, double, double yoffset) {
    scrollWheel += (int32_t)yoffset;
}

GLFWwindow* Minecraft::window = Util::getGLFWWindow();
shared_ptr<Textures> Minecraft::textures;

Minecraft::Minecraft(int32_t width, int32_t height, bool fullscreen) 
    : width(width), height(height), fullscreen(fullscreen) {
    textures = make_shared<Textures>();
    textures->registerTextureFX(make_shared<TextureLavaFX>());
    textures->registerTextureFX(make_shared<TextureWaterFX>());
}

void Minecraft::setScreen(shared_ptr<Screen> screen) {
    if (!dynamic_pointer_cast<ErrorScreen>(this->screen)) {
        if (this->screen != nullptr) {
            this->screen->closeScreen();
        }

        this->screen = screen;
        if (screen != nullptr) {
            if (this->mouseGrabbed) {
                this->player->releaseAllKeys();
                this->mouseGrabbed = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            int32_t width = this->width * 240 / this->height;
            int32_t height = this->height * 240 / this->height;
            shared_ptr<Minecraft> shared = shared_from_this();
            screen->init(shared, width, height);
            hideGui = false;
        }
        else {
            this->grabMouse();
        }
    }
}

void Minecraft::checkGlError(string where) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        cerr << "GL ERROR @ " << where << " : " << err << " (" << (const char*)gluErrorString(err) << ")\n";
        running = false;
        throw runtime_error("GL error: " + to_string((int)err));
    }
}

void Minecraft::destroy() {
    if (soundPlayer != nullptr) {
        soundPlayer->running = false;
        soundPlayer.reset();
    }

    soundManager.reset();
    
    if (backgroundDownloader != nullptr) {
        backgroundDownloader->closing = true;
    }

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
        Util::win = nullptr;
    }

    glfwTerminate();
}

void Minecraft::run() {
    running = true;
    try {
        if (!glfwInit()) {
            throw runtime_error("Failed to initialize GLFW");
        }
        shared_ptr<Minecraft> mcshared = shared_from_this();
        loadingScreen = make_shared<ProgressListener>(mcshared);
        renderHelper = make_unique<RenderHelper>(mcshared);
        levelIo = make_unique<LevelIO>(loadingScreen);
        levelGen = make_unique<LevelGen>(loadingScreen);
        if (fullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
            this->width = vidMode->width;
            this->height = vidMode->height;
            window = glfwCreateWindow(width, height, "", monitor, nullptr);
        }
        else {
            window = glfwCreateWindow(width, height, "", nullptr, nullptr);
        }

        if (!window) {
            glfwTerminate();
            throw runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowTitle(window, "Minecraft 0.0.23a_01");
        glfwSwapInterval(0);

        glfwSetScrollCallback(window, scroll_callback);
        checkGlError("Pre startup");
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glCullFace(GL_BACK);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        checkGlError("Startup");
        font = make_shared<Font>("resources/textures/default.gif", textures);
        glViewport(0, 0, width, height);
        if (!server.empty() && user != nullptr) {
            level = nullptr;
        }
        else {
            bool levelCreated = false;
            try {
                if (!loadMapUser.empty()) {
                    levelCreated = loadLevel(loadMapUser, loadMapId);
                }

                gzFile mine = gzopen("level.mine", "rb");
                gzFile dat = gzopen("level.dat", "rb");
                if (mine) {
                    level = levelIo->load(mine);
                    levelCreated = level != nullptr;
                } 
                else if (dat) {
                    level = levelIo->loadDat(dat);
                    levelCreated = level != nullptr;
                    if (!levelCreated) {
                        level = levelIo->loadLegacy(gzopen("level.dat", "rb"));
                    }
                }

                levelCreated = level != nullptr;
                if (levelCreated) {
                    setLevel(level);
                }
            } catch (const exception& e) {
                cerr << e.what() << endl;
                levelCreated = false;
            }

            if (!levelCreated) {
                generateLevel(1);
            }
        }
        

        levelRenderer = make_unique<LevelRenderer>(textures);
        particleEngine = make_shared<ParticleEngine>(level, textures);
        options = make_unique<Options>(mcshared, "");
        shared_ptr<MovementInputFromOptions> movementInputFromOptions = make_shared<MovementInputFromOptions>(options.get());
        player = make_unique<Player>(level, movementInputFromOptions);
        player->resetPos();
        if (level != nullptr) {
            setLevel(level);
        }
        soundManager = make_unique<SoundManager>();
        soundPlayer = make_shared<SoundPlayer>(options.get());
        
        
        backgroundDownloader = make_unique<BackgroundDownloader>("resources/sounds", mcshared);
        checkGlError("Post startup");
        hud = make_shared<InGameHud>(mcshared, width, height);
        if(!server.empty() && user != nullptr) {
            connectionManager = new ConnectionManager(mcshared, server, port, user->name, user->mpPass);
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
        tinyfd_messageBox("Failed to start Minecraft", e.what(), "ok", "error", 1);
        return;
    }

    int64_t lastTime = Timer::nanoTime() / 1000000;
    int32_t fps = 0;

    try {
        while (running) {
            glfwPollEvents();
            if (pause) {
                this_thread::sleep_for(chrono::milliseconds(100));
            }
            else {
                if (glfwWindowShouldClose(window)) {
                    running = false;
                }

                try {
                    int64_t now = Timer::nanoTime() / 1000000;
                    int64_t passedMS = now - timer.lastSyncSysClock;
                    int64_t var11 = Timer::nanoTime() / 1000000L;
                    if (passedMS > 1000) {
                        int64_t var13 = var11 - timer.lastSyncHRClock;
                        double var15 = (double)passedMS / (double)var13;
                        timer.timeSyncAdjustment += (var15 - timer.timeSyncAdjustment) * 0.2;
                        timer.lastSyncSysClock = now;
                        timer.lastSyncHRClock = var11;
                    }

                    if (passedMS < 0) {
                        timer.lastSyncSysClock = now;
                        timer.lastSyncHRClock = var11;
                    }

                    double var48 = (double)(var11) / 1000.0;
                    double var15 = (var48 - timer.lastHRTime) * timer.timeSyncAdjustment;
                    timer.lastHRTime = var48;

                    if (var15 < 0.0) {
                        var15 = 0.0;
                    }

                    if (var15 > 1.0) {
                        var15 = 1.0;
                    }
                    
                    timer.fps = (float)((double)timer.fps + var15 * (double)timer.timeScale * (double)timer.ticksPerSecond);
                    timer.ticks = (int32_t)timer.fps;
                    if (timer.ticks > 100) {
                        timer.ticks = 100;
                    }

                    timer.fps -= (float)timer.ticks;
                    timer.a = timer.fps;

                    for (int32_t i = 0; i < timer.ticks; i++) {
                        ticksRan++;
                        tick();
                    }

                    checkGlError("Pre render");
                    float a = timer.a;
                    if (renderHelper->displayActive && !Util::windowIsActive()) {
                        pauseGame();
                    }

                    renderHelper->displayActive = Util::windowIsActive();
                    if (mouseGrabbed) {
                        double xo = 0.0f;
                        double yo = 0.0f;
                        glfwGetCursorPos(window, &xo, &yo);
                        xo = xo - width / 2;
                        yo = yo - height / 2;
                        glfwSetCursorPos(window, width / 2, height / 2);
                        float invert = 1;
                        if (options->invertMouse) {
                            invert = -1;
                        }
                        player->turn(xo, -yo * invert);
                    }

                    if (!hideGui) {
                        int32_t width = this->width * 240 / this->height;
                        int32_t height = this->height * 240 / this->height;
                        double mouseX;
                        double mouseY;
                        glfwGetCursorPos(window, &mouseX, &mouseY);
                        int32_t mouseX1 = mouseX * width / this->width;
                        int32_t mouseY1 = mouseY * height / this->height;
                        if (level != nullptr) {
                            render(timer.a);
                            hud->render(screen != nullptr, mouseX1, mouseY1);
                            checkGlError("Rendered gui");
                        }
                        else {
                            glViewport(0, 0, width, height);
                            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                            glMatrixMode(GL_PROJECTION);
                            glLoadIdentity();
                            glMatrixMode(GL_MODELVIEW);
                            glLoadIdentity();
                            renderHelper->initGui();
                        }

                        if (screen) {
                            screen->render(mouseX1, mouseY1);
                        }

                        this_thread::yield();
                        glfwSwapBuffers(window);
                    }
                    
                    checkGlError("Post render");
                    fps++;

                    while (Timer::nanoTime() / 1000000 >= lastTime + 1000) {
                        fpsString = to_string(fps) + " fps, " + to_string(Chunk::updates) + " chunk updates";
                        Chunk::updates = 0;
                        lastTime += 1000;
                        fps = 0;
                    }
                } catch (const exception& e) {
                    std::string msg = "The game broke! [" + std::string(e.what()) + "]";
                    setScreen(make_shared<ErrorScreen>("Client error", msg));
                }
            }
        }
        if (level != nullptr) {
            levelIo->save(level, gzopen("level.mine", "wb"));
        }

        return;
    } catch (const StopGameException& e) {
        return;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    destroy();
}

void Minecraft::grabMouse() {
    if (!mouseGrabbed) {
        mouseGrabbed = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        setScreen(nullptr);
        prevFrameTime = ticksRan + 10000;
    }
}

void Minecraft::pauseGame() {
    if (!dynamic_pointer_cast<PauseScreen>(screen)) {
        setScreen(make_shared<PauseScreen>());
    }
}

void Minecraft::clickMouse() {
    if (hitResult.has_value()) {
        int32_t x = hitResult->x;
        int32_t y = hitResult->y;
        int32_t z = hitResult->z;
        if (editMode != 0) {
            if (hitResult->face == 0) {
                y--;
            }

            if (hitResult->face == 1) {
                y++;
            }

            if (hitResult->face == 2) {
                z--;
            }

            if (hitResult->face == 3) {
                z++;
            }

            if (hitResult->face == 4) {
                x--;
            }

            if (hitResult->face == 5) {
                x++;
            }
        }
        

        Tile* tile = Tile::tiles[level->getTile(x, y, z)];
        if (editMode == 0) {
            if (tile != Tile::unbreakable || player->userType >= 100) {
                bool changed = level->netSetTile(x, y, z, 0);
                if (tile != nullptr && changed) {
                    if (isMultiplayer()) {
                        connectionManager->sendBlockChange(x, y, z, editMode, player->inventory->getSelected());
                    }

                    Tile::SoundType var12 = tile->soundType;
                    auto it = Tile::soundTypeMap.find(var12);
                    if (it != Tile::soundTypeMap.end()) {
                        if(var12 != Tile::SoundType::NONE) {
                            const auto& soundData = it->second;
                            level->playSound("step." + soundData.name, (float)x, (float)y, (float)z, (soundData.getVolume() + 1.0f) / 2.0f, soundData.getPitch() * 0.8f);
                            tile->destroy(level, x, y, z, particleEngine);
                        }
                    }
                }

                return;
            }
        }
        else {
            int32_t selectedTile = player->inventory->getSelected();
            Tile* tile = Tile::tiles[level->getTile(x, y, z)];
            if (tile == nullptr || tile == Tile::water || tile == Tile::calmWater || tile == Tile::lava || tile == Tile::calmLava) {
                optional<AABB> aabb = Tile::tiles[selectedTile]->getTileAABB(x, y, z);
                if(!aabb.has_value() || (player->bb.intersects(aabb.value()) ? false : level->isFree(aabb.value()))) {
                    if (isMultiplayer()) {
                        connectionManager->sendBlockChange(x, y, z, editMode, selectedTile);
                    }
                    level->netSetTile(x, y, z, player->inventory->getSelected());
                    Tile::tiles[selectedTile]->onBlockAdded(level, x, y, z);
                }
            }
        }
    }
}

void Minecraft::tick() {
    if (soundPlayer != nullptr) {
        if (Timer::nanoTime() / 1000000 > soundManager->lastMusic && soundManager->playMusic(soundPlayer, "calm")) {
            soundManager->lastMusic = Timer::nanoTime() / 1000000 + soundManager->random.nextInt(900000) + 300000;
        }
    }

    for (int32_t i = 0; i < hud->messages.size(); ++i) {
        hud->messages[i].counter++;
    }

    glBindTexture(GL_TEXTURE_2D, this->textures->getTextureId("resources/textures/terrain.png"));

    for(int32_t i = 0; i < textures->textureList.size(); i++) {
        shared_ptr<TextureFX> textureFx = textures->textureList[i];
        textureFx->onTick();
        textures->textureBuffer.clear();
        this->textures->textureBuffer.insert(this->textures->textureBuffer.end(), textureFx->imageData.data(), textureFx->imageData.data() + textureFx->imageData.size());
        glTexSubImage2D(GL_TEXTURE_2D, 0, textureFx->iconIndex % 16 << 4, textureFx->iconIndex / 16 << 4, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, textures->textureBuffer.data());
    }


    if (connectionManager != nullptr && !dynamic_pointer_cast<ErrorScreen>(screen)) {
        if (!connectionManager->isConnected()) {
            loadingScreen->beginLevelLoading("Connecting..");
            loadingScreen->setLoadingProgress(0);
        }
        else {
            if (connectionManager->processData) {
                if (connectionManager->connection->connected) {
                    try {
                        connectionManager->connection->processDataFunc();
                    } catch (const exception& e) {
                        setScreen(make_shared<ErrorScreen>("Disconnected!", "You\'ve lost connection to the server"));
                        hideGui = false;
                        cout << e.what() << endl;
                        connectionManager->connection->disconnect();
                        connectionManager = nullptr;
                    }
                }
            }
            if (connectionManager && connectionManager->connection && connectionManager->connected) {
                int32_t x = (int32_t)(player->x * 32.0F);
                int32_t y = (int32_t)(player->y * 32.0F);
                int32_t z = (int32_t)(player->z * 32.0F);
                int32_t yRot = (int32_t)(player->yRot * 256.0F / 360.0F) & 255;
                int32_t xRot = (int32_t)(player->xRot * 256.0F / 360.0F) & 255;
                connectionManager->connection->sendPacket(Packet::PLAYER_TELEPORT, {(int8_t)-1, (int16_t)x, (int16_t)y, (int16_t)z, (int8_t)yRot, (int8_t)xRot});
            }
        }
    }

    int32_t var2;
    if (!screen || screen->allowUserInput) {
        while (true) {
            int32_t var1;
            if (!screen && Util::isMouseKeyDown(GLFW_MOUSE_BUTTON_LEFT) && (float)(ticksRan - prevFrameTime) >= timer.ticksPerSecond / 4.0f && mouseGrabbed) {
                clickMouse();
                prevFrameTime = ticksRan;
            }

            static vector<bool> lastState(348);
            for (int32_t k = 0; k < 348; ++k) {
                bool state = Util::isKeyDown(k);
                if (state != lastState[k]) {
                    lastState[k] = state;
                    player->setKey(k, state);
                }
            }

            if (screen) {
                screen->updateKeyboardEvents();
            }

            if (!screen) {
                if (Util::isKeyDownPrev(GLFW_KEY_ESCAPE)) {
                    pauseGame();
                }

                if (Util::isKeyDownPrev(options->load.key)) {
                    player->resetPos();
                }

                if (Util::isKeyDownPrev(options->save.key)) {
                    level->setSpawnPos((int32_t)player->x, (int32_t)player->y, (int32_t)player->z, player->yRot);
                    player->resetPos();
                }

                for (int32_t i = 0; i < 9; i++) {
                    if (Util::isKeyDownPrev(GLFW_KEY_1 + i)) {
                        player->inventory->selectedSlot = i;
                    }
                }

                if (Util::isKeyDownPrev(GLFW_KEY_Y)) {
                    options->invertMouse = !options->invertMouse;
                }

                if (Util::isKeyDownPrev(GLFW_KEY_G) && connectionManager == nullptr && level->entities.size() < 256) {
                    shared_ptr<Zombie> zombie = make_shared<Zombie>(level, player->x, player->y, player->z);
                    level->entities.push_back(zombie);
                }

                if (Util::isKeyDownPrev(options->toggleFog.key)) {
                    bool isShiftUp = !Util::isKeyDown(GLFW_KEY_LEFT_SHIFT) && !Util::isKeyDown(GLFW_KEY_RIGHT_SHIFT);
                    options->setOption(4, isShiftUp ? 1 : -1);
                }

                if (Util::isKeyDownPrev(options->build.key)) {
                    setScreen(make_shared<InventoryScreen>());
                }

                if (Util::isKeyDownPrev(options->chat.key) && connectionManager && connectionManager->isConnected()) {
                    player->releaseAllKeys();
                    setScreen(make_shared<ChatScreen>());
                }
            }
            
            var1 = scrollWheel;
            scrollWheel = 0;
            
            if (var1 != 0) {
                player->inventory->scrollHotbar(var1);
            }

            if (!screen) {
                if (!mouseGrabbed) {
                    grabMouse();
                }
                else {
                    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT) && mouseGrabbed) {
                        clickMouse();
                        prevFrameTime = ticksRan;
                    }

                    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_RIGHT)) {
                        editMode = (editMode + 1) % 2;
                    }

                    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_MIDDLE) && hitResult.has_value()) {
                        int32_t tile = level->getTile(hitResult->x, hitResult->y, hitResult->z);
                        if (tile == Tile::grass->id) {
                            tile = Tile::dirt->id;
                        }

                        shared_ptr<Inventory> inventory = player->inventory;
                        int32_t slot = inventory->containsTileAt(tile);
                        if (slot >= 0) {
                            inventory->selectedSlot = slot;
                        }
                        else if (tile > 0 && find(User::getCreativeTiles().begin(), User::getCreativeTiles().end(), Tile::tiles[tile]) != User::getCreativeTiles().end()) {
                            inventory->getSlotContainsTile(Tile::tiles[tile]);
                        }
                    }
                }
            }

            break;
        }
    }

    if (screen) {
        prevFrameTime = ticksRan + 10000;
        screen->updateEvents();
        if (screen) {
            screen->tick();
        }
    }
    
    if (level != nullptr) {
        levelRenderer->cloudTickCounter++;
        level->tickEntities();
        if (!isMultiplayer()) {
            level->tick();
        }

        particleEngine->tick();
        player->tick();
    }
}

void Minecraft::render(float a) {    
    glViewport(0, 0, width, height);
    float var18 = 1.0f / (float)(4 - options->renderDistance);
    var18 = (float)pow((double)var18, 0.25);
    renderHelper->fogColorRed = 0.6f * (1.0f - var18) + var18;
    renderHelper->fogColorGreen = 0.8f * (1.0f - var18) + var18;
    renderHelper->fogColorBlue = 1.0f * (1.0f - var18) + var18;
    renderHelper->fogColorRed *= renderHelper->fogColorMultiplier;
    renderHelper->fogColorGreen *= renderHelper->fogColorMultiplier;
    renderHelper->fogColorBlue *= renderHelper->fogColorMultiplier;
    Tile* tile = Tile::tiles[level->getTile((int32_t)player->x, (int32_t)(player->y + 0.12f), (int32_t)player->z)];
    if (tile && tile->getLiquidType() != Liquid::none) {
        Liquid* liquid = tile->getLiquidType();
        if (liquid == Liquid::water) {
            renderHelper->fogColorRed = 0.02f;
            renderHelper->fogColorGreen = 0.02f;
            renderHelper->fogColorBlue = 0.2f;
        }
        else if (liquid == Liquid::lava) {
            renderHelper->fogColorRed = 0.6;
            renderHelper->fogColorGreen = 0.1f;
            renderHelper->fogColorBlue = 0.0f;
        }
    }

    glClearColor(renderHelper->fogColorRed, renderHelper->fogColorGreen, renderHelper->fogColorBlue, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("Set viewport");
    float interpolatedPitchRot = player->xRotO + (player->xRot - player->xRotO) * a;
    float interpolatedYawRot = player->yRotO + (player->yRot - player->yRotO) * a;
    float interpolatedX = player->xo + (player->x - player->xo) * a;
    float interpolatedY = player->yo + (player->y - player->yo) * a;
    float interpolatedZ = player->zo + (player->z - player->zo) * a;
    Vec3 playerPos = Vec3(interpolatedX, interpolatedY, interpolatedZ);
    float yawCos = (float)cos((double)(-interpolatedYawRot) * M_PI / 180.0 + M_PI);
    float yawSin = (float)sin((double)(-interpolatedYawRot) * M_PI / 180.0 + M_PI);
    float pitchCos = (float)cos((double)(-interpolatedPitchRot) * M_PI / 180.0);
    float pitchSin = (float)sin((double)(-interpolatedPitchRot) * M_PI / 180.0);
    yawSin *= pitchCos;
    yawCos *= pitchCos;
    float raycastDistance = 5.0F;
    float dirX = yawSin * raycastDistance;
    float dirY = pitchSin * raycastDistance;
    float dirZ = yawCos * raycastDistance;
    Vec3 var14 = Vec3(playerPos.x + dirX, playerPos.y + dirY, playerPos.z + dirZ);
    hitResult = level->clip(playerPos, var14);
    checkGlError("Picked");
    renderHelper->fogColorMultiplier = 1.0F;
    renderHelper->renderDistance = float(512 >> (options->renderDistance << 1));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, float(width) / height, 0.05f, renderHelper->renderDistance);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -0.3f);
	glRotatef(player->xRotO + (player->xRot - player->xRotO) * a, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRotO + (player->yRot - player->yRotO) * a, 0.0f, 1.0f, 0.0f);
	float var2 = player->xo + (player->x - player->xo) * a;
	float var3 = player->yo + (player->y - player->yo) * a;
	float var4 = player->zo + (player->z - player->zo) * a;
	glTranslatef(-var2, -var3, -var4);
    checkGlError("Set up camera");
    glEnable(GL_CULL_FACE);
    Frustum& frustum = Frustum::getFrustum();
    for (Chunk* c : levelRenderer->sortedChunks) {
        c->isInFrustumFunc(frustum);
    }

    vector<Chunk*> dirty(levelRenderer->dirtyChunks.begin(), levelRenderer->dirtyChunks.end());
    DirtyChunkSorter sorter(player);
    sort(dirty.begin(), dirty.end(), sorter);

    int32_t rebuildCount = 4;
    for (Chunk* c : dirty) {
        c->rebuild();
        levelRenderer->dirtyChunks.erase(c);
        if (--rebuildCount == 0) {
            break;
        }
    }

    checkGlError("Update chunks");
    bool var21 = level->isSolid(player->x, player->y, player->z, 0.1f);
    renderHelper->setupFog();
    glEnable(GL_FOG);
    levelRenderer->render(player, 0);
    if (var21) {
        int32_t px = (int32_t)player->x;
        int32_t py = (int32_t)player->y;
        int32_t pz = (int32_t)player->z;
        for (int32_t x = px - 1; x <= px + 1; ++x) {
            for (int32_t y = py - 1; y <= py + 1; ++y) {
                for (int32_t z = pz - 1; z <= pz + 1; ++z) {
                    levelRenderer->render(x, y, z);
                }
            }  
        }
    }

    checkGlError("Rendered level");
    renderHelper->toggleLight(true);
    levelRenderer->renderEntities(frustum, a);
    renderHelper->toggleLight(false);
    renderHelper->setupFog();
    checkGlError("Rendered entities");
    particleEngine->render(player.get(), a);
    checkGlError("Rendered particles");
    glCallList(levelRenderer->surroundLists);
    glDisable(GL_LIGHTING);
    renderHelper->setupFog();
    levelRenderer->renderClouds(a);
    renderHelper->setupFog();
    glEnable(GL_LIGHTING);
    if (hitResult.has_value()) {
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(player, hitResult, editMode, player->inventory->getSelected());
        levelRenderer->renderHitOutline(hitResult, editMode);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_LIGHTING);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderHelper->setupFog();
    glCallList(levelRenderer->surroundLists + 1);
    glEnable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    int32_t var22 = levelRenderer->render(player, 1);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if(var22 > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures->getTextureId("resources/textures/terrain.png"));
        glCallLists(levelRenderer->dummyBuffer.size(), GL_INT, levelRenderer->dummyBuffer.data());
        glDisable(GL_TEXTURE_2D);
    }
    
    glDepthMask(true);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    if (hitResult.has_value()) {
        glDepthFunc(GL_LESS);
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(player, hitResult, editMode, player->inventory->getSelected());
        levelRenderer->renderHitOutline(hitResult, editMode);
        glEnable(GL_ALPHA_TEST);
        glDepthFunc(GL_LEQUAL);
    }
}

bool Minecraft::isMultiplayer() {
    return connectionManager != nullptr;
}

void Minecraft::generateLevel(int32_t var1) {
    string var2 = user != nullptr ? user->name : "anonymous";
    shared_ptr<Level> generatedLevel = levelGen->generateLevel(var2, 128 << var1, 128 << var1, 64);
    setLevel(generatedLevel);
}

bool Minecraft::loadLevel(string var1, int32_t var2) {
    shared_ptr<Level> var3 = levelIo->load(minecraftUri, var1, var2);
    bool var4 = var3 != nullptr;
    if(!var4) {
        return false;
    } else {
        setLevel(var3);
        return true;
    }
}

void Minecraft::setLevel(shared_ptr<Level> level) {
    this->level = level;
    if (level != nullptr) {
        level->rendererContext = shared_from_this();
    }

    if (levelRenderer != nullptr) {
        shared_ptr<Level> rendererLevel = levelRenderer->level.lock();
        if (rendererLevel != nullptr) {
            rendererLevel->removeListener(levelRenderer.get());
        }

        levelRenderer->level = level;
        if (level != nullptr) {
            level->addListener(levelRenderer.get());
            levelRenderer->compileSurroundingGround();
        }
    }

    if(particleEngine != nullptr) {
        particleEngine->particles.clear();
    }

    if(player != nullptr) {
        player->setLevel(level);
        player->resetPos();
    }
}