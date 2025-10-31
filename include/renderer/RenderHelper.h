#pragma once

#include <memory>
#include <atomic>
#include <vector>

using namespace std;

class Minecraft;

class RenderHelper {
public:
    weak_ptr<Minecraft> minecraft;
    float fogColorMultiplier = 1.0f;
    bool displayActive = false;
    float fogColorRed = 0.5f;
    float fogColorGreen = 0.8f;
    float fogColorBlue = 1.0f;
    float renderDistance = 0.0f;
private:
    atomic<int32_t> unusedInt1 = 0;
    atomic<int32_t> unusedInt2 = 0;
    vector<float> lb = vector<float>(16);
public:
    RenderHelper(shared_ptr<Minecraft>& minecraft);

    void toggleLight(bool toggleLight);
    void initGui();
    void setupFog();
private:
    vector<float>& getBuffer(float a, float b, float c, float d);
};