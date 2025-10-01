#include <iostream>
#include <vector>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "Textures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Util.h"

using namespace std;

unordered_map<string, int32_t>* Textures::idMap = new unordered_map<string, int32_t>();

int32_t Textures::loadTexture(string resourceName, int32_t mode) {
    try {
        if (idMap->find(resourceName) != idMap->end()) {
            return idMap->at(resourceName);
        }
        vector<GLuint> ib(1);
        glGenTextures(1, ib.data());
        int32_t id = ib[0];
        idMap->insert(std::pair<std::string, std::int32_t>(resourceName, id));
        cout << resourceName + " -> " + to_string(id) << endl;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
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