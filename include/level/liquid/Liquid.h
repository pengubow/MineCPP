#pragma once

#include <memory>
#include <vector>

using namespace std;

class Liquid {
private:
    static vector<Liquid*> liquids;
public:
    static Liquid* none;
    static Liquid* water;
    static Liquid* lava;
private:
    Liquid(int32_t id);
};

