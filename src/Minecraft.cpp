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
        fogColor0 = {fogColorRed, fogColorGreen, fogColorBlue, 1.0F};
        fogColor1 = {(float)14 / 255.0F, (float)11 / 255.0F, (float)10 / 255.0F, 1.0F};
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
        glfwSetWindowTitle(window, "Minecraft 0.0.20a_02");
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
        font = make_shared<Font>("default.gif", textures);
        glViewport(0, 0, width, height);

        shared_ptr<Minecraft> mcshared = shared_from_this();
        levelIo = make_shared<LevelIO>(mcshared);
        levelGen = make_shared<LevelGen>(mcshared);
        if (!server.empty() && user != nullptr) {
            shared_ptr<Minecraft> shared = shared_from_this();
            connectionManager = new ConnectionManager(shared, server, port, user->name, user->mpPass);
            level = nullptr;
        }
        else {
            bool levelCreated = false;
            try {
                if (!loadMapUser.empty()) {
                    cerr << "no" << endl;
                    // var9 = loadLevel(loadMapUser, loadMapId);
                }

                shared_ptr<Level> level;
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
                        levelCreated = level != nullptr;
                    }
                }
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
        

        levelRenderer = make_shared<LevelRenderer>(textures);
        particleEngine = make_shared<ParticleEngine>(level, textures);
        shared_ptr<MovementInputFromOptions> shared = make_shared<MovementInputFromOptions>();
        player = make_shared<Player>(level, shared);
        player->resetPos();
        if (level != nullptr) {
            setLevel(level);
        }

        checkGlError("Post startup");
        hud = make_shared<InGameHud>(mcshared, width, height);
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
                    timer.partialTicks = timer.fps;

                    for (int32_t i = 0; i < timer.ticks; i++) {
                        ticksRan++;
                        tick();
                    }

                    checkGlError("Pre render");
                    float partialTicks = timer.partialTicks;
                    if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
                        pauseGame();
                    }

                    if (mouseGrabbed) {
                        double xo = 0.0f;
                        double yo = 0.0f;
                        glfwGetCursorPos(window, &xo, &yo);
                        xo = xo - width / 2;
                        yo = yo - height / 2;
                        glfwSetCursorPos(window, width / 2, height / 2);
                        player->turn(xo, -yo * (float)yMouseAxis);
                    }

                    if (!hideGui) {
                        if (level != nullptr) {
                            render(timer.partialTicks);
                            hud->render();
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
                            initGui();
                        }

                        if (screen) {
                            int32_t width = this->width * 240 / this->height;
                            int32_t height = this->height * 240 / this->height;
                            double mouseX;
                            double mouseY;
                            glfwGetCursorPos(window, &mouseX, &mouseY);
                            int32_t mouseX1 = mouseX * width / this->width;
                            int32_t mouseY1 = mouseY * height / this->height;
                            screen->render(mouseX1, mouseY1);
                        }

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
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    destroy();
}

void Minecraft::grabMouse() {
    if(!mouseGrabbed) {
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

                    tile->destroy(level, x, y, z, particleEngine);
                }

                return;
            }
        }
        else {
            int32_t selectedTile = player->inventory->getSelected();
            Tile* tile = Tile::tiles[level->getTile(x, y, z)];
            if (tile == nullptr || tile == Tile::water || tile == Tile::calmWater || tile == Tile::lava || tile == Tile::calmLava) {
                optional<AABB> aabb = Tile::tiles[selectedTile]->getAABB(x, y, z);
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
    for (int32_t i = 0; i < hud->messages.size(); ++i) {
        hud->messages[i].counter++;
    }

    glBindTexture(GL_TEXTURE_2D, this->textures->getTextureId("terrain.png"));

    for(int32_t i = 0; i < textures->textureList.size(); i++) {
        shared_ptr<TextureFX> textureFx = textures->textureList[i];
        textureFx->onTick();
        textures->textureBuffer.clear();
        this->textures->textureBuffer.insert(this->textures->textureBuffer.end(), textureFx->imageData.data(), textureFx->imageData.data() + textureFx->imageData.size());
        glTexSubImage2D(GL_TEXTURE_2D, 0, textureFx->iconIndex % 16 << 4, textureFx->iconIndex / 16 << 4, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, textures->textureBuffer.data());
    }

    if (connectionManager != nullptr) {
        if (!connectionManager->isConnected()) {
            beginLevelLoading("Connecting..");
            setLoadingProgress(0);
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
    if (screen != nullptr) {
        prevFrameTime = ticksRan + 10000;
    }
    else {
        while (true) {
            int32_t var1;
            
            var1 = scrollWheel;
            scrollWheel = 0;
            
            if (var1 != 0) {
                var2 = var1;
                shared_ptr<Inventory> inventory = player->inventory;
                if (var1 > 0) {
                    var2 = 1;
                }

                if (var2 < 0) {
                    var2 = -1;
                }

                for(inventory->selectedSlot -= var2; inventory->selectedSlot < 0; inventory->selectedSlot += inventory->slots.size()) {}

                while (inventory->selectedSlot >= inventory->slots.size()) {
                    inventory->selectedSlot -= inventory->slots.size();
                }
            }

            if (!mouseGrabbed) {
                grabMouse();
            }
            else {
                if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT)) {
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
                    int32_t slot = inventory->getSlotContainsID(tile);
                    if (slot >= 0) {
                        inventory->selectedSlot = slot;
                    }
                    else if (tile > 0 && find(User::getCreativeTiles().begin(), User::getCreativeTiles().end(), Tile::tiles[tile]) != User::getCreativeTiles().end()) {
                        inventory->getSlotContainsTile(Tile::tiles[tile]);
                    }
                }
            }

            static vector<bool> lastState(348);
            for (int32_t k = 0; k < 348; ++k) {
                bool state = Util::isKeyDown(k);
                if (state != lastState[k]) {
                    lastState[k] = state;
                    player->setKey(k, state);
                }
            }

            if (Util::isKeyDownPrev(GLFW_KEY_ESCAPE)) {
                pauseGame();
            }

            if (Util::isKeyDownPrev(GLFW_KEY_R)) {
                player->resetPos();
            }

            if (Util::isKeyDownPrev(GLFW_KEY_ENTER)) {
                level->setSpawnPos((int32_t)player->x, (int32_t)player->y, (int32_t)player->z, player->yRot);
                player->resetPos();
            }

            for (int32_t i = 0; i < 9; i++) {
                if (Util::isKeyDownPrev(GLFW_KEY_1 + i)) {
                    player->inventory->selectedSlot = i;
                }
            }

            if (Util::isKeyDownPrev(GLFW_KEY_Y)) {
                yMouseAxis = -yMouseAxis;
            }

            if (Util::isKeyDownPrev(GLFW_KEY_G) && connectionManager == nullptr && level->entities.size() < 256) {
                shared_ptr<Zombie> zombie = make_shared<Zombie>(level, player->x, player->y, player->z);
                level->entities.push_back(zombie);
            }

            if (Util::isKeyDownPrev(GLFW_KEY_F)) {
                bool isShiftDown = Util::isKeyDown(GLFW_KEY_LEFT_SHIFT) || Util::isKeyDown(GLFW_KEY_RIGHT_SHIFT);
                levelRenderer->drawDistance = levelRenderer->drawDistance + (isShiftDown ? -1 : 1) & 3;
            }

            if (Util::isKeyDownPrev(GLFW_KEY_B)) {
                setScreen(make_shared<InventoryScreen>());
            }

            if (Util::isKeyDownPrev(GLFW_KEY_T) && connectionManager && connectionManager->isConnected()) {
                player->releaseAllKeys();
                setScreen(make_shared<ChatScreen>());
            }

            if (Util::isMouseKeyDown(GLFW_MOUSE_BUTTON_LEFT) && (float)(ticksRan - prevFrameTime) >= timer.ticksPerSecond / 4.0F && mouseGrabbed) {
                clickMouse();
                prevFrameTime = ticksRan;
            }
            break;
        }
    }

    if (screen != nullptr) {
        screen->updateEvents();
        if (screen != nullptr) {
            screen->tick();
        }
    }
    if(level != nullptr) {
        levelRenderer->cloudTickCounter++;
        level->tickEntities();
        if (!isMultiplayer()) {
            level->tick();
        }
        for (var2 = 0; var2 < particleEngine->particles.size(); ++var2) {
            shared_ptr<Particle> particle = particleEngine->particles[var2];
            particle->tick();
            if (particle->removed) {
                particleEngine->particles.erase(particleEngine->particles.begin() + var2);
                var2--;
            }
        }

        player->tick();
    }
}

bool Minecraft::isMultiplayer() {
    return connectionManager != nullptr;
}

void Minecraft::orientCamera(float a) {
    glTranslatef(0.0F, 0.0F, -0.3F);
	glRotatef(player->xRotO + (player->xRot - player->xRotO) * a, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRotO + (player->yRot - player->yRotO) * a, 0.0f, 1.0f, 0.0f);
	float var2 = player->xo + (player->x - player->xo) * a;
	float var3 = player->yo + (player->y - player->yo) * a;
	float var4 = player->zo + (player->z - player->zo) * a;
	glTranslatef(-var2, -var3, -var4);
}

void Minecraft::render(float a) {    
    glViewport(0, 0, width, height);
    float var18 = 1.0f / (float)(4 - levelRenderer->drawDistance);
    var18 = (float)pow((double)var18, 0.25);
    fogColorRed = 0.6f * (1.0f - var18) + var18;
    fogColorGreen = 0.8f * (1.0f - var18) + var18;
    fogColorBlue = 1.0f * (1.0f - var18) + var18;
    fogColorRed *= fogColorMultiplier;
    fogColorGreen *= fogColorMultiplier;
    fogColorBlue *= fogColorMultiplier;
    Tile* tile = Tile::tiles[level->getTile((int32_t)player->x, (int32_t)(player->y + 0.12f), (int32_t)player->z)];
    if (tile && tile->getLiquidType() != Liquid::none) {
        Liquid* liquid = tile->getLiquidType();
        if (liquid == Liquid::water) {
            fogColorRed = 0.02f;
            fogColorGreen = 0.02f;
            fogColorBlue = 0.2f;
        }
        else if (liquid == Liquid::lava) {
            fogColorRed = 0.6;
            fogColorGreen = 0.1f;
            fogColorBlue = 0.0f;
        }
    }

    glClearColor(fogColorRed, fogColorGreen, fogColorBlue, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("Set viewport");
    fill(selectBuffer.begin(), selectBuffer.end(), 0);
    glSelectBuffer((GLsizei)selectBuffer.size(), selectBuffer.data());
    glRenderMode(GL_SELECT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewportBuffer.data());
    gluPickMatrix((float)(width / 2), (float)(height / 2), 5.0f, 5.0f, viewportBuffer.data());
    gluPerspective(70.0f, (float)width / height, 0.2f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    orientCamera(a);
    Frustum& frustum = Frustum::getFrustum();
    shared_ptr<Tesselator> t = Tesselator::instance;
    float expand = 2.5f;
    AABB aabb = player->bb.grow(expand, expand, expand);
    int32_t x0 = (int32_t)(aabb.minX);
    int32_t x1 = (int32_t)(aabb.maxX + 1.0f);
    int32_t y0 = (int32_t)(aabb.minY);
    int32_t y1 = (int32_t)(aabb.maxY + 1.0f);
    int32_t z0 = (int32_t)(aabb.minZ);
    int32_t z1 = (int32_t)(aabb.maxZ + 1.0f);
    shared_ptr<Level> lRlevel = levelRenderer->level.lock();
    if (!lRlevel) {
        return;
    }
    glInitNames();
    glPushName(0);
    glPushName(0);

    for (int32_t x = x0; x < x1; x++) {
        glLoadName(x);
        glPushName(0);

        for (int32_t y = y0; y < y1; y++) {
            glLoadName(y);
            glPushName(0);

            for (int32_t z = z0; z < z1; ++z) {
                Tile* tile = Tile::tiles[lRlevel->getTile(x, y, z)];
                optional<AABB> aabb = Tile::getTileAABB(x, y, z);
                if (tile && tile->mayPick() && aabb.has_value() && frustum.isVisible(aabb.value())) {
                    glLoadName(z);
                    glPushName(0);

                    for (int32_t face = 0; face < 6; ++face) {
                        if (Tile::cullFace(lRlevel, x, y, z, face)) {
                            glLoadName(face);
                            t->begin();
                            Tile::renderFaceNoTexture(player, t, x, y, z, face);
                            t->end();
                        }
                    }
                    glPopName();
                }
            }
            glPopName();
        }
        glPopName();
    }

    glPopName();
    glPopName();
    
    GLint hits = glRenderMode(GL_RENDER);

    array<int32_t, 10> names;
    optional<HitResult> closestHit = nullopt;
    int32_t readPos = 0;
    for (int32_t i = 0; i < hits; ++i) {
        GLuint numNames = selectBuffer[readPos++];
        readPos++;
        readPos++;

        for (int32_t n = 0; n < numNames; ++n) {
            names[n] = static_cast<int32_t>(selectBuffer[readPos++]);
        }

        hitResult = HitResult(names[0], names[1], names[2], names[3], names[4]);

        if (closestHit.has_value()) {
            float distNew = hitResult->distanceTo(player.get(), editMode);
            float distOld = closestHit->distanceTo(player.get(), editMode);
            if (distNew >= distOld) {
                continue;
            }
        }
        closestHit = hitResult;
    }

    hitResult = closestHit;
    checkGlError("Picked");
    fogColorMultiplier = 1.0F;
    renderDistance = float(512 >> (levelRenderer->drawDistance << 1));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, float(width) / height, 0.05f, renderDistance);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    orientCamera(a);
    checkGlError("Set up camera");
    glEnable(GL_CULL_FACE);
    frustum = Frustum::getFrustum();
    for (shared_ptr<Chunk> c : levelRenderer->sortedChunks) {
        c->isInFrustumFunc(frustum);
    }

    vector<shared_ptr<Chunk>> dirty(levelRenderer->dirtyChunks.begin(), levelRenderer->dirtyChunks.end());
    DirtyChunkSorter sorter(player);
    sort(dirty.begin(), dirty.end(), sorter);

    int32_t rebuildCount = 4;
    for (shared_ptr<Chunk> c : dirty) {
        c->rebuild();
        levelRenderer->dirtyChunks.erase(c);
        if (--rebuildCount == 0) {
            break;
        }
    }

    checkGlError("Update chunks");
    bool var21 = level->isSolid(player->x, player->y, player->z, 0.1f);
    setupFog();
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
    toggleLight(true);
    levelRenderer->renderEntities(frustum, a);
    toggleLight(false);
    setupFog();
    checkGlError("Rendered entities");
    particleEngine->render(player.get(), a);
    checkGlError("Rendered particles");
    glCallList(levelRenderer->surroundLists);
    glDisable(GL_LIGHTING);
    setupFog();
    levelRenderer->renderClouds(a);
    setupFog();
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
    setupFog();
    glCallList(levelRenderer->surroundLists + 1);
    glEnable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    int32_t var22 = levelRenderer->render(player, 1);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if(var22 > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures->getTextureId("terrain.png"));
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

void Minecraft::toggleLight(bool toggleLight) {
    if (!toggleLight) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
    else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        float var4 = 0.7f;
        float var2 = 0.3f;
        Vec3 var3 = Vec3(0.0f, -1.0f, 0.5f).normalize();
        glLightfv(GL_LIGHT0, GL_POSITION, getBuffer(var3.x, var3.y, var3.z, 0.0f).data());
        glLightfv(GL_LIGHT0, GL_DIFFUSE, getBuffer(var2, var2, var2, 1.0f).data());
        glLightfv(GL_LIGHT0, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f).data());
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(var4, var4, var4, 1.0f).data());
    }
}

void Minecraft::initGui() {
    int32_t width = this->width * 240 / this->height;
    int32_t height = this->height * 240 / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
}

void Minecraft::setupFog() {
    glFogfv(GL_FOG_COLOR, getBuffer(fogColorRed, fogColorGreen, fogColorBlue, 1.0f).data());
    glNormal3f(0.0f, -1.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    Tile* tile = Tile::tiles[level->getTile((int32_t)player->x, (int32_t)(player->y + 0.12f), (int32_t)player->z)];
    if (tile != nullptr && tile->getLiquidType() != Liquid::none) {
        Liquid* liquid = tile->getLiquidType();
        glFogi(GL_FOG_MODE, GL_EXP);
        if (liquid == Liquid::water) {
            glFogf(GL_FOG_DENSITY, 0.1f);
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.4f, 0.9f, 1.0f).data());
        }
        else if(liquid == Liquid::lava) {
            glFogf(GL_FOG_DENSITY, 2.0f);
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.3f, 0.3f, 1.0f).data());
        }
    }
    else {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0.0f);
        glFogf(GL_FOG_END, renderDistance);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f).data());
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glEnable(GL_LIGHTING);
}

vector<float>& Minecraft::getBuffer(float a, float b, float c, float d) {
    this->lb.clear();
    this->lb.reserve(4);
    this->lb.push_back(a);
    this->lb.push_back(b);
    this->lb.push_back(c);
    this->lb.push_back(d);
    return this->lb;
}

void Minecraft::beginLevelLoading(string title) {
    if (!running) {
        throw StopGameException();
    }
    else {
        this->title = title;
        int32_t width = this->width * 240 / this->height;
        int32_t height = this->height * 240 / this->height;
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)width, (double)height, 0.0, 100.0, 300.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -200.0f);
    }
}

void Minecraft::levelLoadUpdate(string text) {
    if (!running) {
        throw StopGameException();
    }
    else {
        this->text = text;
        setLoadingProgress(-1);
    }
}

void Minecraft::setLoadingProgress(int32_t var1) {
    if (!running) {
        throw StopGameException();
    }
    else {
        int32_t width = this->width * 240 / this->height;
        int32_t height = this->height * 240 / this->height;
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        shared_ptr<Tesselator> t = Tesselator::instance;
        glEnable(GL_TEXTURE_2D);
        int32_t var5 = textures->getTextureId("dirt.png");
        glBindTexture(GL_TEXTURE_2D, var5);
        float var8 = 32.0F;
        t->begin();
        t->color(4210752);
        t->vertexUV(0.0F, (float)height, 0.0f, 0.0f, (float)height / var8);
        t->vertexUV((float)width, (float)height, 0.0f, (float)width / var8, (float)height / var8);
        t->vertexUV((float)width, 0.0f, 0.0f, (float)width / var8, 0.0f);
        t->vertexUV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        t->end();
        if(var1 >= 0) {
            var5 = width / 2 - 50;
            int32_t var6 = height / 2 + 16;
            glDisable(GL_TEXTURE_2D);
            t->begin();
            t->color(8421504);
            t->vertex((float)var5, (float)var6, 0.0f);
            t->vertex((float)var5, (float)(var6 + 2), 0.0f);
            t->vertex((float)(var5 + 100), (float)(var6 + 2), 0.0f);
            t->vertex((float)(var5 + 100), (float)var6, 0.0f);
            t->color(8454016);
            t->vertex((float)var5, (float)var6, 0.0f);
            t->vertex((float)var5, (float)(var6 + 2), 0.0f);
            t->vertex((float)(var5 + var1), (float)(var6 + 2), 0.0f);
            t->vertex((float)(var5 + var1), (float)var6, 0.0f);
            t->end();
            glEnable(GL_TEXTURE_2D);
        }

        font->drawShadow(title, (width - font->width(title)) / 2, height / 2 - 4 - 16, 16777215);
        font->drawShadow(text, (width - font->width(text)) / 2, height / 2 - 4 + 8, 16777215);
        glfwSwapBuffers(window);

        this_thread::yield();
    }
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

void Minecraft::addChatMessage(string message) {
    hud->messages.push_front(ChatLine(message));

    while (hud->messages.size() > 50) {
        hud->messages.pop_back();
    }
}