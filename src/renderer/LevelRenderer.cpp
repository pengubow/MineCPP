#include <algorithm>
#include <iostream>
#include "renderer/LevelRenderer.h"
#include "renderer/DistanceSorter.h"

LevelRenderer::LevelRenderer(shared_ptr<Textures>& textures) 
    : textures(textures), surroundLists(glGenLists(2)),
    chunkRenderLists(glGenLists(4096 << 6 << 1)) {}

void LevelRenderer::compileSurroundingGround() {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }

    int32_t var1;
    if (!sortedChunks.empty()) {
        for(var1 = 0; var1 < sortedChunks.size(); ++var1) {
            sortedChunks[var1]->clear();
        }
    }

    xChunks = level->width / 16;
    yChunks = level->depth / 16;
    zChunks = level->height / 16;
    sortedChunks = vector<shared_ptr<Chunk>>(xChunks * yChunks * zChunks);
    chunks = vector<shared_ptr<Chunk>>(xChunks * yChunks * zChunks);

    var1 = 0;

    int var4;
    for(int var2 = 0; var2 < xChunks; ++var2) {
        for(int var3 = 0; var3 < yChunks; ++var3) {
            for(var4 = 0; var4 < zChunks; ++var4) {
                sortedChunks[(var4 * yChunks + var3) * xChunks + var2] = make_shared<Chunk>(level, var2 << 4, var3 << 4, var4 << 4, 16, chunkRenderLists + var1);
                chunks[(var4 * yChunks + var3) * xChunks + var2] = sortedChunks[(var4 * yChunks + var3) * xChunks + var2];
                var1 += 2;
            }
        }
    }

    dirtyChunks.clear();
    glNewList(surroundLists, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->textures->getTextureId("rock.png"));
    float var10 = 0.5f;
    glColor4f(var10, var10, var10, 1.0f);
    shared_ptr<Tesselator> t = Tesselator::instance;
    float groundLevel = level->getGroundLevel();
    int32_t var5 = 128;
    if (128 > level->width) {
        var5 = level->width;
    }

    if (var5 > level->height) {
        var5 = level->height;
    }

    int32_t var6 = 2048 / var5;
    t->begin();
    int32_t var7;
    for (var7 = -var5 * var6; var7 < level->width + var5 * var6; var7 += var5) {
        for (int32_t var8 = -var5 * var6; var8 < level->height + var5 * var6; var8 += var5) {
            var10 = groundLevel;
            if (var7 >= 0 && var8 >= 0 && var7 < level->width && var8 < level->height) {
                var10 = 0.0f;
            }

            t->vertexUV((float)var7, var10, (float)(var8 + var5), 0.0f, (float)var5);
            t->vertexUV((float)(var7 + var5), var10, (float)(var8 + var5), (float)var5, (float)var5);
            t->vertexUV((float)(var7 + var5), var10, (float)var8, (float)var5, 0.0f);
            t->vertexUV((float)var7, var10, (float)var8, 0.0f, 0.0f);
        }
    }

    t->end();
    glBindTexture(GL_TEXTURE_2D, this->textures->getTextureId("rock.png"));
    glColor3f(0.8f, 0.8f, 0.8f);
    t->begin();

    for (var7 = 0; var7 < level->width; var7 += var5) {
        t->vertexUV((float)var7, 0.0f, 0.0f, 0.0f, 0.0f);
        t->vertexUV((float)(var7 + var5), 0.0f, 0.0f, (float)var5, 0.0f);
        t->vertexUV((float)(var7 + var5), groundLevel, 0.0f, (float)var5, groundLevel);
        t->vertexUV((float)var7, groundLevel, 0.0f, 0.0f, groundLevel);
        t->vertexUV((float)var7, groundLevel, (float)(level->height), 0.0f, groundLevel);
        t->vertexUV((float)(var7 + var5), groundLevel, (float)(level->height), (float)var5, groundLevel);
        t->vertexUV((float)(var7 + var5), 0.0f, (float)(level->height), (float)var5, 0.0f);
        t->vertexUV((float)var7, 0.0f, (float)(level->height), 0.0f, 0.0f);
    }

    glColor3f(0.6f, 0.6f, 0.6f);

    for (var7 = 0; var7 < level->height; var7 += var5) {
        t->vertexUV(0.0f, groundLevel, (float)var7, 0.0f, 0.0f);
        t->vertexUV(0.0f, groundLevel, (float)(var7 + var5), (float)var5, 0.0f);
        t->vertexUV(0.0f, 0.0f, (float)(var7 + var5), (float)var5, groundLevel);
        t->vertexUV(0.0f, 0.0f, (float)var7, 0.0f, groundLevel);
        t->vertexUV((float)(level->width), 0.0f, (float)var7, 0.0f, groundLevel);
        t->vertexUV((float)(level->width), 0.0f, (float)(var7 + var5), (float)var5, groundLevel);
        t->vertexUV((float)(level->width), groundLevel, (float)(var7 + var5), (float)var5, 0.0F);
        t->vertexUV((float)(level->width), groundLevel, (float)var7, 0.0f, 0.0f);
    }

    t->end();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEndList();
    glNewList(surroundLists + 1, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0F, 1.0F, 1.0F);
    glBindTexture(GL_TEXTURE_2D, this->textures->getTextureId("water.png"));
    var10 = level->getWaterLevel();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int32_t var12 = 128;
    if(128 > level->width) {
        var12 = level->width;
    }

    if(var12 > level->height) {
        var12 = level->height;
    }

    var5 = 2048 / var12;
    t->begin();

    for(var6 = -var12 * var5; var6 < level->width + var12 * var5; var6 += var12) {
        for(var7 = -var12 * var5; var7 < level->height + var12 * var5; var7 += var12) {
            float var13 = var10 - 0.1f;
            if(var6 < 0 || var7 < 0 || var6 >= level->width || var7 >= level->height) {
                t->vertexUV((float)var6, var13, (float)(var7 + var12), 0.0f, (float)var12);
                t->vertexUV((float)(var6 + var12), var13, (float)(var7 + var12), (float)var12, (float)var12);
                t->vertexUV((float)(var6 + var12), var13, (float)var7, (float)var12, 0.0f);
                t->vertexUV((float)var6, var13, (float)var7, 0.0f, 0.0F);
                t->vertexUV((float)var6, var13, (float)var7, 0.0f, 0.0f);
                t->vertexUV((float)(var6 + var12), var13, (float)var7, (float)var12, 0.0f);
                t->vertexUV((float)(var6 + var12), var13, (float)(var7 + var12), (float)var12, (float)var12);
                t->vertexUV((float)var6, var13, (float)(var7 + var12), 0.0f, (float)var12);
            }
        }
    }

    t->end();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEndList();
    setDirty(0, 0, 0, level->width, level->depth, level->height);
}

void LevelRenderer::renderEntities(Frustum& frustum, float var2) {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }

    for (int32_t var3 = 0; var3 < level->entities.size(); ++var3) {
        shared_ptr<Entity> entity = level->entities[var3];
        if (frustum.isVisible(entity->bb)) {
            level->entities[var3]->render(textures, var2);
        }
    }
}

int32_t LevelRenderer::render(shared_ptr<Player>& player, int32_t layer) {
    float var3 = player->x - lX;
    float var4 = player->y - lY;
    float var5 = player->z - lZ;
    if (var3 * var3 + var4 * var4 + var5 * var5 > 64.0f) {
        lX = player->x;
        lY = player->y;
        lZ = player->z;
        sort(chunks.begin(), chunks.end(), DistanceSorter(player));
    }

    dummyBuffer.clear();
    dummyBuffer.reserve(65536);

    for (int32_t var6 = 0; var6 < chunks.size(); ++var6) {
        chunks[var6]->render(dummyBuffer, layer);
    }

    if (!dummyBuffer.empty()) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures->getTextureId("terrain.png"));
        glCallLists(dummyBuffer.size(), GL_INT, dummyBuffer.data());
        glDisable(GL_TEXTURE_2D);
    }

    return dummyBuffer.size();
}

void LevelRenderer::renderClouds(float var1) {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->getTextureId("clouds.png"));
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    shared_ptr<Tesselator> t = Tesselator::instance;
    float var3 = 0.0F;
    float var4 = 0.5F / 1024.0F;
    var3 = (float)(level->depth + 2);
    var1 = ((float)cloudTickCounter + var1) * var4 * 0.03F;
    float var5 = 0.0F;
    t->begin();

    int32_t var8;
    for (var8 = -2048; var8 < level->width + 2048; var8 += 512) {
        for (int32_t var6 = -2048; var6 < level->height + 2048; var6 += 512) {
            t->vertexUV((float)var8, var3, (float)(var6 + 512), (float)var8 * var4 + var1, (float)(var6 + 512) * var4);
            t->vertexUV((float)(var8 + 512), var3, (float)(var6 + 512), (float)(var8 + 512) * var4 + var1, (float)(var6 + 512) * var4);
            t->vertexUV((float)(var8 + 512), var3, (float)var6, (float)(var8 + 512) * var4 + var1, (float)var6 * var4);
            t->vertexUV((float)var8, var3, (float)var6, (float)var8 * var4 + var1, (float)var6 * var4);
            t->vertexUV((float)var8, var3, (float)var6, (float)var8 * var4 + var1, (float)var6 * var4);
            t->vertexUV((float)(var8 + 512), var3, (float)var6, (float)(var8 + 512) * var4 + var1, (float)var6 * var4);
            t->vertexUV((float)(var8 + 512), var3, (float)(var6 + 512), (float)(var8 + 512) * var4 + var1, (float)(var6 + 512) * var4);
            t->vertexUV((float)var8, var3, (float)(var6 + 512), (float)var8 * var4 + var1, (float)(var6 + 512) * var4);
        }
    }

    t->end();
    glDisable(GL_TEXTURE_2D);
    t->begin();
    t->color(0.5F, 0.8F, 1.0F);
    var3 = (float)(level->depth + 10);

    for (int32_t var7 = -2048; var7 < level->width + 2048; var7 += 512) {
        for (var8 = -2048; var8 < level->height + 2048; var8 += 512) {
            t->vertex((float)var7, var3, (float)var8);
            t->vertex((float)(var7 + 512), var3, (float)var8);
            t->vertex((float)(var7 + 512), var3, (float)(var8 + 512));
            t->vertex((float)var7, var3, (float)(var8 + 512));
        }
    }

    t->end();
}

void LevelRenderer::render(int32_t x, int32_t y, int32_t z) {
    shared_ptr<Level> level = this->level.lock();
    if (!level) {
        return;
    }

    int32_t var6 = level->getTile(x, y, z);
    if (var6 != 0 && Tile::tiles[var6]->isSolid()) {
        glEnable(GL_TEXTURE_2D);
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        glDepthFunc(GL_LESS);
        shared_ptr<Tesselator> t = Tesselator::instance;
        t->begin();

        int32_t var5;
        for (var5 = 0; var5 < 6; ++var5) {
            Tile::tiles[var6]->renderFace(t, x, y, z, var5);
        }

        t->end();
        glCullFace(GL_FRONT);
        t->begin();

        for (var5 = 0; var5 < 6; ++var5) {
            Tile::tiles[var6]->renderFace(t, x, y, z, var5);
        }

        t->end();
        glCullFace(GL_BACK);
        glDisable(GL_TEXTURE_2D);
        glDepthFunc(GL_LEQUAL);
    }
}

void LevelRenderer::renderHit(shared_ptr<Player>& player, optional<HitResult>& h, int32_t mode, int32_t tileType) {
    shared_ptr<Tesselator> t = Tesselator::instance;
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 1.0f, 1.0f, ((float)sin((double)Timer::nanoTime() / 1000000 / 100.0) * 0.2f + 0.4f) * 0.5f);
    if (mode == 0) {
        t->begin();
        for (int32_t i = 0; i < 6; ++i) {
            Tile::renderFaceNoTexture(player, t, h->x, h->y, h->z, i);
        }
        t->end();
    }
    else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float var8 = (float)sin((double)Timer::nanoTime() / 1000000 / 100.0) * 0.2f + 0.8f;
        glColor4f(var8, var8, var8, (float)sin((double)Timer::nanoTime() / 1000000 / 200.0) * 0.2f + 0.5f);
        glEnable(GL_TEXTURE_2D);
        int32_t id = textures->getTextureId("terrain.png");
        glBindTexture(GL_TEXTURE_2D, id);
        int32_t x = h->x;
        int32_t y = h->y;
        int32_t z = h->z;
        if (h->face == 0) {
            y--;
        }

        if (h->face == 1) {
            y++;
        }

        if (h->face == 2) {
            z--;
        }

        if (h->face == 3) {
            z++;
        }

        if (h->face == 4) {
            x--;
        }

        if (h->face == 5) {
            x++;
        }

        t->begin();
        t->setNoColor();
        shared_ptr<Level> level = this->level.lock();
        if (!level) {
            return;
        }

        Tile::tiles[tileType]->render(t, level, 0, x, y, z);
        Tile::tiles[tileType]->render(t, level, 1, x, y, z);
        t->end();
        glDisable(GL_TEXTURE_2D);
    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

void LevelRenderer::renderHitOutline(optional<HitResult>& h, int32_t mode) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    float x = (float)(h->x);
    float y = (float)(h->y);
    float z = (float)(h->z);
    if (mode == 1) {
        if (h->face == 0) {
            y--;
        }

        if (h->face == 1) {
            y++;
        }

        if (h->face == 2) {
            z--;
        }

        if (h->face == 3) {
            z++;
        }

        if (h->face == 4) {
            x--;
        }

        if (h->face == 5) {
            x++;
        }
    }

    glBegin(GL_LINE_STRIP);
    glVertex3f(x, y, z);
    glVertex3f(x + 1.0f, y, z);
    glVertex3f(x + 1.0f, y, z + 1.0f);
    glVertex3f(x, y, z + 1.0f);
    glVertex3f(x, y, z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(x, y + 1.0f, z);
    glVertex3f(x + 1.0f, y + 1.0f, z);
    glVertex3f(x + 1.0f, y + 1.0f, z + 1.0f);
    glVertex3f(x, y + 1.0f, z + 1.0f);
    glVertex3f(x, y + 1.0f, z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x, y, z);
    glVertex3f(x, y + 1.0f, z);
    glVertex3f(x + 1.0f, y, z);
    glVertex3f(x + 1.0f, y + 1.0f, z);
    glVertex3f(x + 1.0f, y, z + 1.0f);
    glVertex3f(x + 1.0f, y + 1.0f, z + 1.0f);
    glVertex3f(x, y, z + 1.0f);
    glVertex3f(x, y + 1.0f, z + 1.0f);
    glEnd();
    glDisable(GL_BLEND);
}

void LevelRenderer::setDirty(int32_t minX, int32_t minY, int32_t minZ, int32_t maxX, int32_t maxY, int32_t maxZ) {
    minX /= 16;
    minY /= 16;
    minZ /= 16;
    maxX /= 16;
    maxY /= 16;
    maxZ /= 16;
    if (minX < 0) {
        minX = 0;
    }

    if (minY < 0) {
        minY = 0;
    }

    if (minZ < 0) {
        minZ = 0;
    }

    if (maxX > xChunks - 1) {
        maxX = xChunks - 1;
    }

    if (maxY > yChunks - 1) {
        maxY = yChunks - 1;
    }

    if (maxZ > zChunks - 1) {
        maxZ = zChunks - 1;
    }

    for (int32_t x = minX; x <= maxX; x++) {
        for (int32_t y = minY; y <= maxY; y++) {
            for (int32_t z = minZ; z <= maxZ; z++) {
                dirtyChunks.insert(sortedChunks[(z * yChunks + y) * xChunks + x]);
            }
        }
    }

}