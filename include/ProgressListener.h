#pragma once

#include <string>
#include <memory>
#include "renderer/Tesselator.h"

class Minecraft;

class ProgressListener {
    string title = "";
    weak_ptr<Minecraft> minecraft;
    string text = "";
public:
    ProgressListener(shared_ptr<Minecraft>& minecraft);

    void beginLevelLoading(string text);
    void levelLoadUpdate(string title);
    void setLoadingProgress(int32_t var1);
};