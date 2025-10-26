#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>
#include <memory>
#include <GL/gl.h>
#include "renderer/texture/TextureFX.h"

using namespace std;

class Textures {
    unordered_map<string, int32_t> idMap = unordered_map<string, int32_t>();
public:
    vector<GLuint> idBuffer = vector<GLuint>(1);
    vector<uint8_t> textureBuffer = vector<uint8_t>(262144);
    vector<shared_ptr<TextureFX>> textureList;
    
    int32_t getTextureId(string resourceName);
    int32_t addTexture(uint8_t* image, int32_t width, int32_t height);
    void registerTextureFX(shared_ptr<TextureFX> textureFx);
};