#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>
#include "RubyDung.h"

RubyDung::RubyDung() {}

void RubyDung::init() {
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
        throw std::runtime_error("Failed to init GLFW");
    }

    if (FULLSCREEN_MODE) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
        this->width = vidMode->width;
        this->height = vidMode->height;
        window = glfwCreateWindow(width, height, "Game", monitor, nullptr);
    }
    else {
        window = glfwCreateWindow(width, height, "Game", nullptr, nullptr);
    }
    
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int32_t w, h;
    glfwGetFramebufferSize(window, &w, &h);
    width = w; height = h;

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glClearColor(fr, fg, fb, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    level = std::make_shared<Level>(256, 256, 64);
    levelRenderer = std::make_shared<LevelRenderer>(level);
    player = std::make_shared<Player>(level);
    particleEngine = std::make_shared<ParticleEngine>(level);

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    lastMouseX = mx; lastMouseY = my;

    
    for (int32_t i = 0; i < 10; i++) {
        std::shared_ptr<Zombie> zombie = std::make_shared<Zombie>(this->level, 128.0f, 0.0f, 128.0f);
        zombie->resetPos();
        this->zombies.push_back(zombie);
    }
}

void RubyDung::destroy() {
    if (level) {
        level->save();
    }

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void RubyDung::tick() {
    if (Util::isKeyDownPrev(GLFW_KEY_ENTER)) {
        this->level->save();
    }
    if (Util::isKeyDownPrev(GLFW_KEY_1)) {
        this->paintTexture = 1;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_2)) {
        this->paintTexture = 3;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_3)) {
        this->paintTexture = 4;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_4)) {
        this->paintTexture = 5;
    }
    if (Util::isKeyDown(GLFW_KEY_6)) {
        this->paintTexture = 6;
    }
    if (Util::isKeyDownPrev(GLFW_KEY_G)) {
        std::shared_ptr<Zombie> zombie = std::make_shared<Zombie>(this->level, this->player->x, this->player->y, this->player->z);
        this->zombies.push_back(zombie);
    }
    this->level->tick();
    this->particleEngine->tick();
    for (int32_t i = 0; i < this->zombies.size(); i++) {
        this->zombies.at(i)->tick();
        if (this->zombies[i]->removed) {
            this->zombies.erase(this->zombies.begin() + i);
            i--;
        }
    }
    this->player->tick();
}

void RubyDung::moveCameraToPlayer(float a) {
    glTranslatef(0.0f, 0.0f, -0.3f);
    glRotatef(player->xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRot, 0.0f, 1.0f, 0.0f);
    float x = player->xo + (player->x - player->xo) * a;
    float y = player->yo + (player->y - player->yo) * a;
    float z = player->zo + (player->z - player->zo) * a;
    glTranslatef(-x, -y, -z);
}

void RubyDung::setupCamera(float a) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, static_cast<double>(width) / static_cast<double>(height), 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->moveCameraToPlayer(a); 
}

void RubyDung::setupPickCamera(float a, int32_t x, int32_t y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewportBuffer.data());
    gluPickMatrix(x, y, 5.0, 5.0, viewportBuffer.data());
    gluPerspective(70.0, (float)width / (float)(height), 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->moveCameraToPlayer(a);
}

void RubyDung::pick(float a) {
    selectBuffer.assign(selectBuffer.size(), 0);
    glSelectBuffer(static_cast<GLsizei>(selectBuffer.size()), selectBuffer.data());
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
        hitResult = std::make_unique<HitResult>(names[0], names[1], names[2], names[3], names[4]);
    } else {
        hitResult = nullptr;
    }
}

void RubyDung::render(float a) {
    glfwPollEvents();
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    mouseDX = mx - lastMouseX;
    mouseDY = my - lastMouseY;
    lastMouseX = mx;
    lastMouseY = my;

    player->turn(mouseDX, -mouseDY);
    this->pick(a);
    
    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_RIGHT) && hitResult) { 
        Tile* oldTile = Tile::tiles[this->level->getTile(this->hitResult->x, this->hitResult->y, this->hitResult->z)];
        bool changed = this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, 0);
        if (oldTile != nullptr && changed) {
            oldTile->destroy(this->level, this->hitResult->x, this->hitResult->y, this->hitResult->z, this->particleEngine);
        }
    } 

    if (Util::isMouseKeyDownPrev(GLFW_MOUSE_BUTTON_LEFT) && hitResult) { 
        int32_t x = hitResult->x; 
        int32_t y = hitResult->y; 
        int32_t z = hitResult->z; 
        if (hitResult->face == 0) {
            --y;
        } 
        if (hitResult->face == 1) {
            ++y;
        } 
        if (hitResult->face == 2) {
            --z;
        } 
        if (hitResult->face == 3) {
            ++z;
        } 
        if (hitResult->face == 4) {
            --x;
        }
        if (hitResult->face == 5) {
            ++x;
        }
        this->level->setTile(x, y, z, this->paintTexture);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->setupCamera(a);
    glEnable(GL_CULL_FACE);
    Frustum& frustum = Frustum::getFrustum();
    this->levelRenderer->updateDirtyChunks(this->player);
    this->setupFog(0);
    glEnable(GL_FOG);

    levelRenderer->render(player, 0);
    
    for (int32_t i = 0; i < this->zombies.size(); i++) {
        std::shared_ptr<Zombie> zombie = this->zombies[i];
        if (zombie->isLit() && frustum.isVisible(zombie->bb)) {
            this->zombies[i]->render(a);
        }
    }
    this->particleEngine->render(this->player, a, 0);
    this->setupFog(1);
    levelRenderer->render(player, 1);
    for (int32_t i = 0; i < this->zombies.size(); i++) {
        std::shared_ptr<Zombie> zombie = this->zombies[i];
        if (!zombie->isLit() && frustum.isVisible(zombie->bb)) {
            this->zombies[i]->render(a);
        }
    }
    this->particleEngine->render(this->player, a, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    if (hitResult) {
        glDisable(GL_ALPHA_TEST);
        levelRenderer->renderHit(hitResult);
        glEnable(GL_ALPHA_TEST);
    }
    this->drawGui(a);
    glfwSwapBuffers(window);
}

void RubyDung::drawGui(float a) {
    int32_t screenWidth = this->width * 240 / this->height;
    int32_t screenHeight = this->height * 240 / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)screenWidth, (double)screenHeight, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
    glPushMatrix();
    glTranslatef((float)(screenWidth - 16), 16.0f, 0.0f);
    std::shared_ptr<Tesselator> t = Tesselator::instance;
    glScalef(16.0f, 16.0f, 16.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-1.5f, 0.5f, -0.5f);
    glScalef(-1.0f, -1.0f, 1.0f);
    int32_t id = Textures::loadTexture("terrain.png", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable(GL_TEXTURE_2D);
    t->init();
    Tile::tiles[this->paintTexture]->render(t, this->level, 0, -2, 0, 0);
    t->flush();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
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
}

void RubyDung::setupFog(int32_t i) {
    if (i == 0) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, this->fogColor0);
        glDisable(GL_FOG);
    }
    else if (i == 1) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, (float)0.06f);
        glFogfv(GL_FOG_COLOR, this->fogColor1);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        float br = 0.6f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, this->getBuffer(br, br, br, 1.0f).data());
    }
}

std::vector<float>& RubyDung::getBuffer(float a, float b, float c, float d) {
    this->lb.clear();
    this->lb.reserve(4);
    this->lb.push_back(a);
    this->lb.push_back(b);
    this->lb.push_back(c);
    this->lb.push_back(d);
    return this->lb;
}

void RubyDung::run() {
    try {
        init();
    } catch (const std::exception& e) {
        std::cerr << "Failed to start RubyDung: " << e.what() << std::endl;
        std::exit(1);
    }

    int32_t lastTime = Timer::nanoTime() / 1000000;
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        timer.advanceTime();
        for (int32_t i = 0; i < timer.ticks; ++i) {
            tick();
        }

        render(timer.partialTicks);
        ++frames;

        int32_t now = Timer::nanoTime() / 1000000;
        if (now >= lastTime + 1000.0) {
            std::cout << frames << " fps, " << Chunk::updates << std::endl;
            Chunk::updates = 0;
            lastTime += 1000.0;
            frames = 0;
        }

        if (Util::isKeyDown(GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
    }

    destroy();
}

void RubyDung::checkError() {
    int e = glGetError();
    if (e != 0) {
        const char* s = (const char*)gluErrorString(e);
        throw std::runtime_error(s ? s : "GL error");
    }
}

int main() {
    RubyDung app;
    app.run();
    return 0;
}