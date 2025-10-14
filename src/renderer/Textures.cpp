#include <iostream>
#include <vector>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "renderer/Textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Util/Util.h"

using namespace std;

unordered_map<string, int32_t> Textures::idMap = unordered_map<string, int32_t>();

int32_t Textures::loadTexture(string resourceName, int32_t mode) {
    try {
        if (idMap.find(resourceName) != idMap.end()) {
            return idMap.at(resourceName);
        }
        vector<GLuint> ib(1);
        glGenTextures(1, ib.data());
        int32_t id = ib[0];
        idMap.insert(pair<string, int32_t>(resourceName, id));
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        int32_t width, height, channels;
        uint8_t* image = stbi_load(resourceName.c_str(), &width, &height, &channels, 4);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        return id;
    } catch (const exception& e) {
        cerr << "!!" << endl;
        exit(EXIT_FAILURE);
    }
}