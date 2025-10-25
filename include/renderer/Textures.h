#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

using namespace std;

class Textures {
    unordered_map<string, int32_t> idMap = unordered_map<string, int32_t>();
public:
    vector<GLuint> idBuffer = vector<GLuint>(1);
    
    int32_t getTextureId(string resourceName);
    int32_t addTexture(uint8_t* image, int32_t width, int32_t height);
};