#include "Minecraft.h"
#include "ProgressListener.h"
#include "StopGameException.h"

ProgressListener::ProgressListener(shared_ptr<Minecraft>& minecraft) 
    : minecraft(minecraft) {}

void ProgressListener::beginLevelLoading(string text) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (!minecraft->running) {
        throw StopGameException();
    }
    else {
        this->text = text;
        int32_t width = minecraft->width * 240 / minecraft->height;
        int32_t height = minecraft->height * 240 / minecraft->height;
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)width, (double)height, 0.0, 100.0, 300.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -200.0f);
    }
};

void ProgressListener::levelLoadUpdate(string title) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (!minecraft->running) {
        throw StopGameException();
    }
    else {
        this->title = title;
        setLoadingProgress(-1);
    }
}

void ProgressListener::setLoadingProgress(int32_t var1) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (!minecraft->running) {
        throw StopGameException();
    }
    else {
        int32_t width = minecraft->width * 240 / minecraft->height;
        int32_t height = minecraft->height * 240 / minecraft->height;
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        shared_ptr<Tesselator> t = Tesselator::instance;
        glEnable(GL_TEXTURE_2D);
        int32_t var5 = minecraft->textures->getTextureId("resources/textures/dirt.png");
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

        minecraft->font->drawShadow(text, (width - minecraft->font->width(text)) / 2, height / 2 - 4 - 16, 16777215);
		minecraft->font->drawShadow(title, (width - minecraft->font->width(title)) / 2, height / 2 - 4 + 8, 16777215);
        glfwSwapBuffers(minecraft->window);

        this_thread::yield();
    }
}