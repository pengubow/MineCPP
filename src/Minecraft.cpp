#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include <thread>
#include <algorithm>
#include <set>
#include "Util/tinyfiledialogs.h"
#include "Minecraft.h"
#include "gui/PauseScreen.h"
#include "renderer/DirtyChunkSorter.h"

static int32_t scrollWheel = 0;

void scroll_callback(GLFWwindow*, double, double yoffset) {
    scrollWheel += (int32_t)yoffset;
}

GLFWwindow* Minecraft::window = Util::getGLFWWindow();
shared_ptr<Textures> Minecraft::textures;
std::vector<int32_t> Minecraft::creativeTiles;

Minecraft::Minecraft(int32_t width, int32_t height, bool fullscreen) 
    : width(width), height(height), fullscreen(fullscreen) {
    Minecraft::creativeTiles = {Tile::rock->id, Tile::dirt->id, Tile::stoneBrick->id, Tile::wood->id, Tile::bush->id, Tile::log->id, Tile::leaf->id, Tile::sand->id, Tile::gravel->id};
    textures = make_shared<Textures>();
}

void Minecraft::setScreen(shared_ptr<Screen> screen) {
    if (this->screen != nullptr) {
        this->screen->closeScreen();
    }

    this->screen = screen;
    if (screen != nullptr) {
        int32_t width = this->width * 240 / this->height;
        int32_t height = this->height * 240 / this->height;
        shared_ptr<Minecraft> shared = shared_from_this();
        screen->init(shared, width, height);
    }
}

void Minecraft::checkGlError(string string) {
    int32_t errorCode = glGetError();
    if (errorCode != 0) {
        const GLubyte* errorString = gluErrorString(errorCode);
        cerr << "########## GL ERROR ##########" << endl;
        cerr << "@ " + string << endl;
        cerr << errorCode << ": " 
             << (errorString ? reinterpret_cast<const char*>(errorString) : "Unknown error") 
             << endl;
        exit(0);
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
        /* if(parent != null) {
            Display.setParent(parent);
        } else */ if (fullscreen) {
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
        glfwSetWindowTitle(window, "Minecraft 0.0.14a_08");
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
        bool levelCreated = false;

        shared_ptr<Minecraft> mcshared = shared_from_this();
        levelIo = make_shared<LevelIO>(mcshared);
        levelGen = make_shared<LevelGen>(mcshared);

        try {
            if (!loadMapUser.empty()) {
                cerr << "no" << endl;
                // var9 = loadLevel(loadMapUser, loadMapID);
            }

            shared_ptr<Level> level;
            gzFile mpp = gzopen("level.mpp", "rb");
            gzFile dat = gzopen("level.dat", "rb");
            if (mpp) {
                level = levelIo->load(mpp);
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

        levelRenderer = make_shared<LevelRenderer>(textures);
        particleEngine = make_shared<ParticleEngine>(level, textures);
        shared_ptr<MovementInputFromOptions> shared = make_shared<MovementInputFromOptions>();
        player = make_shared<Player>(level, shared);
        player->resetPos();
        if (level != nullptr) {
            levelRenderer->setLevel(level);
        }

        checkGlError("Post startup");
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

                int64_t now = Timer::nanoTime();
                int64_t passedNS = now - timer.lastTime;
                timer.lastTime = now;
                if (passedNS < 0) {
                    passedNS = 0;
                }

                if (passedNS > 1000000000) {
                    passedNS = 1000000000;
                }

                timer.fps += (float)passedNS * timer.timeScale * timer.ticksPerSecond / 1.0E9f;
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
                render(timer.partialTicks);
                checkGlError("Post render");
                fps++;

                while (Timer::nanoTime() / 1000000 >= lastTime + 1000) {
                    fpsString = to_string(fps) + " fps, " + to_string(Chunk::updates) + " chunk updates";
                    Chunk::updates = 0;
                    lastTime += 1000;
                    fps = 0;
                }
            }
        }
        levelIo->save(level, gzopen("level.mpp", "wb"));

        return;
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
    destroy();
}

void Minecraft::stop() {
    running = false;
}

void Minecraft::grabMouse() {
    if(!mouseGrabbed) {
        mouseGrabbed = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        setScreen(nullptr);
    }
}

void Minecraft::releaseMouse() {
    if(mouseGrabbed) {
        player->releaseAllKeys();
        mouseGrabbed = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        setScreen(make_shared<PauseScreen>());
    }
}

void Minecraft::clickMouse() {
    if (hitResult != nullptr) {
        Tile* oldTile = Tile::tiles[level->getTile(hitResult->x, hitResult->y, hitResult->z)];
        if (editMode == 0) {
            bool changed = level->setTile(hitResult->x, hitResult->y, hitResult->z, 0);
            if (oldTile != nullptr && changed) {
                oldTile->destroy(level, hitResult->x, hitResult->y, hitResult->z, particleEngine);
            }

        }
        else {
            int32_t x = hitResult->x;
            int32_t y = hitResult->y;
            int32_t z = hitResult->z;
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

            Tile* tile = Tile::tiles[level->getTile(x, y, z)];
            if (tile == nullptr || tile == Tile::water || tile == Tile::calmWater || tile == Tile::lava || tile == Tile::calmLava) {
                shared_ptr<AABB> aabb = Tile::tiles[paintTexture]->getAABB(x, y, z);
                if(aabb == nullptr || (player->bb->intersects(aabb) ? false : level->isFree(aabb))) {
                    level->setTile(x, y, z, paintTexture);
                    Tile::tiles[paintTexture]->onBlockAdded(level, x, y, z);
                }
            }

        }
    }
}

void Minecraft::tick() {
    int32_t var2;
    if (screen != nullptr) {
        prevFrameTime = ticksRan + 10000;
    }
    else {
        while (true) {
            int32_t var1;
            
            var1 = scrollWheel;
            scrollWheel = 0;
            
            int32_t var3;
            if (var1 != 0) {
                var2 = var1;
                if (var1 > 0) {
                    var2 = 1;
                }

                if (var2 < 0) {
                    var2 = -1;
                }

                var3 = 0;

                for (int32_t var4 = 0; var4 < creativeTiles.size(); ++var4) {
                    if (creativeTiles[var4] == paintTexture) {
                        var3 = var4;
                    }
                }

                for (var3 += var2; var3 < 0; var3 += creativeTiles.size()) {}

                while (var3 >= creativeTiles.size()) {
                    var3 -= creativeTiles.size();
                }

                paintTexture = creativeTiles[var3];
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

                if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_MIDDLE)) {
                    if (hitResult != nullptr) {
                        var2 = level->getTile(hitResult->x, hitResult->y, hitResult->z);
                        if (var2 == Tile::grass->id) {
                            var2 = Tile::dirt->id;
                        }

                        for (var3 = 0; var3 < creativeTiles.size(); ++var3) {
                            if (var2 == creativeTiles[var3]) {
                                paintTexture = creativeTiles[var3];
                            }
                        }
                    }
                }
            }

            static vector<bool> lastState = vector<bool>(348);
            for (int32_t k = 0; k < 348; ++k) {
                bool state = Util::isKeyDown(k);
                if (state != lastState[k]) {
                    lastState[k] = state;
                    player->setKey(k, state);
                }
            }

            if (Util::isKeyDownPrev(GLFW_KEY_ESCAPE)) {
                releaseMouse();
            }

            if (Util::isKeyDownPrev(GLFW_KEY_R)) {
                player->resetPos();
            }

            if (Util::isKeyDownPrev(GLFW_KEY_ENTER)) {
                level->setSpawnPos((int32_t)player->x, (int32_t)player->y, (int32_t)player->z, player->yRot);
                player->resetPos();
            }

            for (int var1 = 0; var1 < 9; ++var1) {
                if (Util::isKeyDownPrev(GLFW_KEY_1 + var1)) {
                    paintTexture = creativeTiles[var1];
                }
            }


            if (Util::isKeyDownPrev(GLFW_KEY_Y)) {
                yMouseAxis = -yMouseAxis;
            }

            if (Util::isKeyDownPrev(GLFW_KEY_G) && level->entities.size() < 256) {
                shared_ptr<Zombie> zombie = make_shared<Zombie>(level, player->x, player->y, player->z);
                level->entities.push_back(zombie);
            }

            if (Util::isKeyDownPrev(GLFW_KEY_F)) {
                levelRenderer->drawDistance = (levelRenderer->drawDistance + 1) % 4;
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

    levelRenderer->cloudTickCounter++;
    level->tick();

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

void Minecraft::orientCamera(float a) {
    glTranslatef(0.0F, 0.0F, -0.3F);
	glRotatef(player->xRot - player->xRotI * (1.0f - a), 1.0f, 0.0f, 0.0f);
	glRotatef(player->yRot - player->yRotI * (1.0f - a), 0.0f, 1.0f, 0.0f);
	float var2 = player->xo + (player->x - player->xo) * a;
	float var3 = player->yo + (player->y - player->yo) * a;
	float var4 = player->zo + (player->z - player->zo) * a;
	glTranslatef(-var2, -var3, -var4);
}

void Minecraft::render(float a) {
    if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        releaseMouse();
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
    
    glViewport(0, 0, width, height);
    checkGlError("Set viewport");
    fill(selectBuffer.begin(), selectBuffer.end(), 0);
    glSelectBuffer((GLsizei)selectBuffer.size(), selectBuffer.data());
    glRenderMode(GL_SELECT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewportBuffer.data());
    gluPickMatrix((float)(width / 2), (float)(height / 2), 5.0f, 5.0f, viewportBuffer.data());
    gluPerspective(70.0f, (float)width / height, 0.05f, 1024.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    orientCamera(a);
    Frustum& frustum = Frustum::getFrustum();
    shared_ptr<Tesselator> t = Tesselator::instance;
    float expand = 2.5f;
    shared_ptr<AABB> aabb = player->bb->grow(expand, expand, expand);
    int32_t x0 = (int32_t)(aabb->minX);
    int32_t x1 = (int32_t)(aabb->maxX + 1.0f);
    int32_t y0 = (int32_t)(aabb->minY);
    int32_t y1 = (int32_t)(aabb->maxY + 1.0f);
    int32_t z0 = (int32_t)(aabb->minZ);
    int32_t z1 = (int32_t)(aabb->maxZ + 1.0f);
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
                shared_ptr<AABB> aabb = Tile::getTileAABB(x, y, z);
                if (tile && tile->mayPick() && frustum.isVisible(aabb)) {
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
    shared_ptr<HitResult> closestHit;
    int32_t readPos = 0;
    for (int32_t i = 0; i < hits; ++i) {
        GLuint numNames = selectBuffer[readPos++];
        readPos++;
        readPos++;

        for (int32_t n = 0; n < numNames; ++n) {
            names[n] = static_cast<int32_t>(selectBuffer[readPos++]);
        }

        hitResult = std::make_shared<HitResult>(names[0], names[1], names[2], names[3], names[4]);

        if (closestHit != nullptr) {
            float distNew = hitResult->distanceTo(player, editMode);
            float distOld = closestHit->distanceTo(player, editMode);
            if (distNew >= distOld) {
                continue;
            }
        }
        closestHit = hitResult;
    }

    hitResult = closestHit;
    checkGlError("Picked");
    fogColorRed = 0.92f;
    fogColorGreen = 0.98f;
    fogColorBlue = 1.0f;
    glClearColor(fogColorRed, fogColorGreen, fogColorBlue, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    renderDistance = float(1024 >> (levelRenderer->drawDistance << 1));
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

    vector<shared_ptr<Chunk>> dirty = levelRenderer->dirtyChunks;
    DirtyChunkSorter sorter(player);
    sort(dirty.begin(), dirty.end(), sorter);

    int32_t rebuildCount = 4;
    for (shared_ptr<Chunk> c : dirty) {
        c->rebuild();
        auto it = find(levelRenderer->dirtyChunks.begin(), levelRenderer->dirtyChunks.end(), c);
        if (it != levelRenderer->dirtyChunks.end()) {
            levelRenderer->dirtyChunks.erase(it);
        }
        if (--rebuildCount == 0) {
            break;
        }
    }

    checkGlError("Update chunks");
    bool var21 = level->isSolid(player->x, player->y, player->z, 0.1f);
    setupFog(0);
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
    levelRenderer->renderEntities(frustum, a);
    checkGlError("Rendered entities");
    particleEngine->render(player, a);
    checkGlError("Rendered particles");
    glCallList(levelRenderer->surroundLists);
    glDisable(GL_LIGHTING);
    setupFog(-1);
    levelRenderer->renderClouds(a);
    setupFog(1);
    glEnable(GL_LIGHTING);
    if (hitResult) {
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(player, hitResult, editMode, paintTexture);
        levelRenderer->renderHitOutline(hitResult, editMode);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_LIGHTING);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setupFog(0);
    glCallList(levelRenderer->surroundLists + 1);
    glEnable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    levelRenderer->render(player, 1);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    levelRenderer->render(player, 1);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    if (hitResult) {
        glDepthFunc(GL_LESS);
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(player, hitResult, editMode, paintTexture);
        LevelRenderer::renderHitOutline(hitResult, editMode);
        glEnable(GL_ALPHA_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    int32_t width = this->width * 240 / height;
    int32_t height = this->height * 240 / this->height;
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    int32_t mouseX1 = mouseX * width / this->width;
    int32_t mouseY1 = mouseY * height / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
    checkGlError("GUI: Init");
    glPushMatrix();
    glTranslatef((float)(width - 16), 16.0f, -50.0f);
    glScalef(16.0f, 16.0f, 16.0f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-1.5f, 0.5f, 0.5f);
    glScalef(-1.0f, -1.0f, -1.0f);
    int32_t tex = textures->loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    t->begin();
    Tile::tiles[paintTexture]->render(t, level, 0, -2, 0, 0);
    t->end();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    checkGlError("GUI: Draw selected");
    font->drawShadow("0.0.14a_08", 2, 2, 0xFFFFFF);
    font->drawShadow(fpsString, 2, 12, 0xFFFFFF);
    checkGlError("GUI: Draw text");
    width /= 2;
    height /= 2;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    t->begin();
    t->vertex((float)(width + 1), (float)(height - 4), 0.0f);
    t->vertex((float)width, (float)(height - 4), 0.0f);
    t->vertex((float)width, (float)(height + 5), 0.0f);
    t->vertex((float)(width + 1), (float)(height + 5), 0.0f);
    t->vertex((float)(width + 5), (float)height, 0.0f);
    t->vertex((float)(width - 4), (float)height, 0.0f);
    t->vertex((float)(width - 4), (float)(height + 1), 0.0f);
    t->vertex((float)(width + 5), (float)(height + 1), 0.0f);
    t->end();
    checkGlError("GUI: Draw crosshair");
    if (screen) {
        screen->render(mouseX1, mouseY1);
    }

    checkGlError("Rendered gui");
    glfwSwapBuffers(window);
}

void Minecraft::setupFog(int32_t i) {
    if (i == -1) {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0.0f);
        glFogf(GL_FOG_END, renderDistance);
        glFogfv(GL_FOG_COLOR, getBuffer(fogColorRed, fogColorGreen, fogColorBlue, 1.0f).data());
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f).data());
    }
    else {
        Tile* var2 = Tile::tiles[level->getTile((int32_t)player->x, (int32_t)(player->y + 0.12f), (int32_t)player->z)];
        if (var2 != nullptr && var2->getLiquidType() == 1) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 0.1f);
            glFogfv(GL_FOG_COLOR, getBuffer(0.02f, 0.02f, 0.2f, 1.0f).data());
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.3f, 0.3f, 0.7f, 1.0f).data());
        }
        else if (var2 != nullptr && var2->getLiquidType() == 2) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 2.0f);
            glFogfv(GL_FOG_COLOR, getBuffer(0.6f, 0.1f, 0.0f, 1.0f).data());
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.3f, 0.3f, 1.0f).data());
        }
        else if (i == 0) {
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, renderDistance);
            glFogfv(GL_FOG_COLOR, getBuffer(fogColorRed, fogColorGreen, fogColorBlue, 1.0F).data());
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f).data());
        }
        else if (i == 1) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 0.01f);
            glFogfv(GL_FOG_COLOR, fogColor1.data());
            float var3 = 0.6f;
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(var3, var3, var3, 1.0f).data());
        }

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT);
        glEnable(GL_LIGHTING);
    }
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

void Minecraft::levelLoadUpdate(string text) {
    this->text = text;
    setLoadingProgress(-1);
}

void Minecraft::setLoadingProgress(int32_t var1) {
    int32_t width = this->width * 240 / this->height;
    int32_t height = this->height * 240 / this->height;
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    shared_ptr<Tesselator> t = Tesselator::instance;
    glEnable(GL_TEXTURE_2D);
    int32_t var5 = textures->loadTexture("dirt.png", GL_NEAREST);
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

void Minecraft::generateLevel(int32_t var1) {
    string var2 = user != nullptr ? user->name : "anonymous";
    shared_ptr<Level> generatedLevel = levelGen->generateLevel(var2, 128 << var1, 128 << var1, 64);
    setLevel(generatedLevel);
}

void Minecraft::setLevel(shared_ptr<Level>& level) {
    this->level = level;
    if (levelRenderer != nullptr) {
        levelRenderer->setLevel(level);
    }

    if(particleEngine != nullptr) {
        particleEngine->particles.clear();
    }

    if(player != nullptr) {
        player->setLevel(level);
        player->resetPos();
    }
}

