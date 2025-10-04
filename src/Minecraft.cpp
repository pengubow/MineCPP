#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include <thread>
#include "Minecraft.h"

Minecraft::Minecraft(int32_t width, int32_t height, bool fullscreen) {
    this->width = width;
    this->height = height;
    this->fullscreen = fullscreen;
    textures = make_shared<Textures>();
}

void Minecraft::init() {
    int32_t col0 = 16710650;
    int32_t col1 = 920330;
    float fr = 0.5f;
    float fg = 0.8f;
    float fb = 1.0f;

    fogColor0[0] = (float)((col0 >> 16) & 255) / 255.0f;
    fogColor0[1] = (float)((col0 >> 8) & 255) / 255.0f;
    fogColor0[2] = (float)(col0 & 255) / 255.0f;
    fogColor0[3] = 1.0f;

    fogColor1[0] = (float)((col1 >> 16) & 255) / 255.0f;
    fogColor1[1] = (float)((col1 >> 8) & 255) / 255.0f;
    fogColor1[2] = (float)(col1 & 255) / 255.0f;
    fogColor1[3] = 1.0f;

    if (!glfwInit()) {
        throw runtime_error("Failed to init GLFW");
    }

    string windowTitle = "Minecraft " + VERSION_STRING;

    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
        this->width = vidMode->width;
        this->height = vidMode->height;
        window = glfwCreateWindow(width, height, windowTitle.c_str(), monitor, nullptr);
    }
    else {
        window = glfwCreateWindow(854, 480, windowTitle.c_str(), nullptr, nullptr);
    }
    
    if (!window) {
        glfwTerminate();
        throw runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    checkGlError("Pre Startup");

    //int32_t w, h;
    //glfwGetFramebufferSize(window, &w, &h);
    //width = w; height = h;

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glClearColor(fr, fg, fb, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    checkGlError("Startup");

    level = make_shared<Level>(256, 256, 64);
    levelRenderer = make_shared<LevelRenderer>(level, textures);
    player = make_shared<Player>(level);
    particleEngine = make_shared<ParticleEngine>(level, textures);
    font = make_unique<Font>("default.gif", textures);

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    lastMouseX = mx; lastMouseY = my;

    
    for (int32_t i = 0; i < 10; i++) {
        shared_ptr<Zombie> zombie = make_shared<Zombie>(this->level, this->textures, 128.0f, 0.0f, 128.0f);
        zombie->resetPos();
        this->entities.push_back(zombie);
    }

    grabMouse();

    checkGlError("Post startup");
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
    try {
        level->save();
    } catch (const exception& e) {
        // empty catch block
    }

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Minecraft::grabMouse() {
    if (mouseGrabbed) {
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(window, width / 2, height / 2);
    mouseGrabbed = true;
}

void Minecraft::releaseMouse() {
    if (!mouseGrabbed) {
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseGrabbed = false;
}

void Minecraft::handleMouseClick() {
    if (editMode == 0) {
        if (hitResult != nullptr) {
            Tile* oldTile = Tile::tiles[level->getTile(hitResult->x, hitResult->y, hitResult->z)];
            bool changed = this->level->setTile(hitResult->x, hitResult->y, hitResult->z, 0);
            if (oldTile != nullptr && changed) {
                oldTile->destroy(level, hitResult->x, hitResult->y, hitResult->z, particleEngine);
            }
        }
    } else if (hitResult != nullptr) {
        shared_ptr<AABB> aabb;
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
        if ((aabb = Tile::tiles[paintTexture]->getAABB(x, y, z)) == nullptr || isFree(aabb)) {
            level->setTile(x, y, z, paintTexture);
        }
    }
}

void Minecraft::tick() {
    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT)) {
        if (!mouseGrabbed) {
            grabMouse();
        }
        else {
            handleMouseClick();
        }
    }
    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_RIGHT)) {
        editMode = (editMode + 1) % 2;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_ESCAPE) && !fullscreen) {
        releaseMouse();
    }
    if (Util::isKeyDownPrev(GLFW_KEY_ENTER)) {
        level->save();
    }
    if (Util::isKeyDownPrev(GLFW_KEY_1)) {
        paintTexture = 1;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_2)) {
        paintTexture = 3;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_3)) {
        paintTexture = 4;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_4)) {
        paintTexture = 5;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_6)) {
        paintTexture = 6;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_Y)) {
        yMouseAxis *= -1;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_G)) {
        shared_ptr<Zombie> zombie = make_shared<Zombie>(level, textures, player->x, player->y, player->z);
        entities.push_back(zombie);
    }
    level->tick();
    particleEngine->tick();
    for (int32_t i = 0; i < entities.size(); i++) {
        entities[i]->tick();
        if (entities[i]->removed) {
            entities.erase(entities.begin() + i);
            i--;
        }
    }
    player->tick();
}

bool Minecraft::isFree(shared_ptr<AABB>& aabb) {
    if (player->bb.intersects(*aabb)) {
        return false;
    }
    
    for (int32_t i = 0;i < entities.size(); i++) {
        if (entities[i]->bb.intersects(*aabb)) {
            return false;
        }
    }
    return true;
}

void Minecraft::moveCameraToPlayer(float a) {
    glTranslatef(0.0f, 0.0f, -0.3f);
    glRotatef(player->xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRot, 0.0f, 1.0f, 0.0f);
    float x = player->xo + (player->x - player->xo) * a;
    float y = player->yo + (player->y - player->yo) * a;
    float z = player->zo + (player->z - player->zo) * a;
    glTranslatef(-x, -y, -z);
}

void Minecraft::setupCamera(float a) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)width / (double)height, 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->moveCameraToPlayer(a); 
}

void Minecraft::setupPickCamera(float a, int32_t x, int32_t y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewportBuffer.data());
    gluPickMatrix(x, y, 5.0, 5.0, viewportBuffer.data());
    gluPerspective(70.0, (float)width / (float)(height), 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->moveCameraToPlayer(a);
}

void Minecraft::pick(float a) {
    selectBuffer.assign(selectBuffer.size(), 0);
    glSelectBuffer((GLsizei)(selectBuffer.size()), selectBuffer.data());
    glRenderMode(GL_SELECT);
    this->setupPickCamera(a, width / 2, height / 2);
    levelRenderer->pick(player, Frustum::getFrustum());
    GLint hits = glRenderMode(GL_RENDER);

    uint32_t closest = 0;
    int32_t names[10];
    int32_t hitNameCount = 0;

    int32_t idx = 0;

    for (int32_t i = 0; i < hits; i++) {
        int32_t nameCount = selectBuffer[idx++];
        int64_t minZ  = selectBuffer[idx++];
        idx++;
        int64_t dist = minZ;

        if (dist < closest || i == 0) {
            closest = dist;
            hitNameCount = nameCount;
            for (int32_t j = 0; j < nameCount; j++) {
                names[j] = selectBuffer[idx++];
            }
        } else {
            for (int32_t j = 0; j < nameCount; j++) {
                idx++;
            }
        }
    }

    if (hitNameCount > 0) {
        hitResult = make_unique<HitResult>(names[0], names[1], names[2], names[3], names[4]);
    } else {
        hitResult = nullptr;
    }
}

void Minecraft::render(float a) {
    if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        releaseMouse();
    }
    glViewport(0, 0, width, height);
    if (mouseGrabbed) {
        double xo = 0.0f;
        double yo = 0.0f;
        glfwGetCursorPos(window, &xo, &yo);
        xo = xo - width / 2;
        yo = yo - height / 2;
        glfwSetCursorPos(window, width / 2, height / 2);
        player->turn(xo, -yo * (float)yMouseAxis);
    }
    checkGlError("Set viewport");
    pick(a);
    checkGlError("Picked");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->setupCamera(a);
    checkGlError("Set up camera");
    glEnable(GL_CULL_FACE);
    Frustum& frustum = Frustum::getFrustum();
    this->levelRenderer->updateDirtyChunks(this->player);
    checkGlError("Update chunks");
    this->setupFog(0);
    glEnable(GL_FOG);

    levelRenderer->render(player, 0);
    checkGlError("Rendered level");
    for (int32_t i = 0; i < this->entities.size(); i++) {
        shared_ptr<Entity> entity = this->entities[i];
        if (entity->isLit() && frustum.isVisible(entity->bb)) {
            this->entities[i]->render(a);
        }
    }
    checkGlError("Rendered entities");
    this->particleEngine->render(this->player, a, 0);
    checkGlError("Rendered particles");
    this->setupFog(1);
    levelRenderer->render(player, 1);
    for (int32_t i = 0; i < this->entities.size(); i++) {
        shared_ptr<Entity> zombie = this->entities[i];
        if (!zombie->isLit() && frustum.isVisible(zombie->bb)) {
            this->entities[i]->render(a);
        }
    }
    this->particleEngine->render(this->player, a, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    checkGlError("Rendered rest");
    if (hitResult) {
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(hitResult, editMode, paintTexture);
        glEnable(GL_ALPHA_TEST);
    }
    checkGlError("Rendered hit");
    this->drawGui(a);
    checkGlError("Rendered gui");
    glfwSwapBuffers(window);
}

void Minecraft::drawGui(float a) {
    int32_t screenWidth = this->width * 240 / this->height;
    int32_t screenHeight = this->height * 240 / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)screenWidth, (double)screenHeight, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
    checkGlError("GUI: Init");
    glPushMatrix();
    glTranslatef((float)(screenWidth - 16), 16.0f, 0.0f);
    shared_ptr<Tesselator> t = Tesselator::instance;
    glScalef(16.0f, 16.0f, 16.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-1.5f, 0.5f, -0.5f);
    glScalef(-1.0f, -1.0f, 1.0f);
    int32_t id = textures->loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable(GL_TEXTURE_2D);
    t->init();
    Tile::tiles[this->paintTexture]->render(t, this->level, 0, -2, 0, 0);
    t->flush();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    checkGlError("GUI: Draw selected");
    font->drawShadow(VERSION_STRING, 2, 2, 0xFFFFFF);
    font->drawShadow(fpsString, 2, 12, 0xFFFFFF);
    checkGlError("GUI: Draw text");
    int32_t wc = screenWidth / 2;
    int32_t hc = screenHeight / 2;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    t->init();
    t->vertex(wc + 1, hc - 4, 0.0f);
    t->vertex(wc - 0, hc - 4, 0.0f);
    t->vertex(wc - 0, hc + 5, 0.0f);
    t->vertex(wc + 1, hc + 5, 0.0f);
    t->vertex(wc + 5, hc - 0, 0.0f);
    t->vertex(wc - 4, hc - 0, 0.0f);
    t->vertex(wc - 4, hc + 1, 0.0f);
    t->vertex(wc + 5, hc + 1, 0.0f);
    t->flush();
    checkGlError("GUI: Draw crosshair");
}

void Minecraft::setupFog(int32_t i) {
    if (i == 0) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, this->fogColor0);
        glDisable(GL_FOG);
    }
    else if (i == 1) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, (float)0.01f);
        glFogfv(GL_FOG_COLOR, this->fogColor1);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        float br = 0.6f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, this->getBuffer(br, br, br, 1.0f).data());
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

void Minecraft::run() {
    running = true;
    try {
        init();
    } catch (const exception& e) {
        cerr << "Failed to start Minecraft: " << e.what() << endl;
        exit(1);
    }

    int32_t lastTime = Timer::nanoTime() / 1000000;
    int frames = 0;
    try {
        while (running) {
            glfwPollEvents();
            if (pause) {
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }
            if (glfwWindowShouldClose(window)) {
                stop();
            }
            timer.advanceTime();
            for (int32_t i = 0; i < timer.ticks; ++i) {
                tick();
            }

            

            checkGlError("Pre render");
            render(timer.partialTicks);
            checkGlError("Post render");
            ++frames;

            int32_t now = Timer::nanoTime() / 1000000;
            if (now >= lastTime + 1000.0) {
                fpsString = to_string(frames) + " fps, " + to_string(Chunk::updates) + " chunk updates";
                Chunk::updates = 0;
                lastTime += 1000.0;
                frames = 0;
            }
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
        destroy();
    }

    destroy();
}

void Minecraft::stop() {
    running = false;
}

void Minecraft::checkError() {
    int e = glGetError();
    if (e != 0) {
        const char* s = (const char*)gluErrorString(e);
        throw runtime_error(s ? s : "GL error");
    }
}