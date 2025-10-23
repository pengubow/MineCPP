#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

using namespace std;

class Textures {
    static unordered_map<string, int32_t> idMap;
public:
    int32_t loadTexture(string resourceName, int32_t mode);
};