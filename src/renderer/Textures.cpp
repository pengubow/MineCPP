#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include "GL_compat.h"
#include "renderer/Textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Util/stb_image.h"
#include "Util/Util.h"

using namespace std;

int32_t Textures::getTextureId(string resourceName) {
    try {
        if (idMap.find(resourceName) != idMap.end()) {
            return idMap.at(resourceName);
        }
        int32_t width, height, channels;
        uint8_t* image = stbi_load(resourceName.c_str(), &width, &height, &channels, 4);
        int32_t textureId = addTexture(image, width, height);
        idMap[resourceName] = textureId;
        stbi_image_free(image);
        return textureId;

    } catch (const exception& e) {
        cerr << "!!" << endl;
        exit(EXIT_FAILURE);
    }
}

int32_t Textures::addTexture(uint8_t* image, int32_t width, int32_t height) {
    idBuffer.clear();
    idBuffer.reserve(1);
    glGenTextures(1, idBuffer.data());
    int32_t id = idBuffer[0];
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int32_t dataSize = width * height * 4;
    textureBuffer.clear();
    textureBuffer.insert(textureBuffer.end(), image, image + dataSize);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer.data());
    return id;
}

void Textures::registerTextureFX(shared_ptr<TextureFX> textureFx) {
    textureList.push_back(textureFx);
    textureFx->onTick();
}