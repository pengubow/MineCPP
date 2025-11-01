#pragma once

#include "renderer/Tesselator.h"
#include "gui/Font.h"

class Gui {
protected:
    float zLevel = 0.0f;

    static void fill(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4);
    static void fillGradient(int32_t var0, int32_t var1, int32_t x, int32_t y, int32_t var4, int32_t var5);
public:
    static void drawCenteredString(shared_ptr<Font>& font, string msg, int32_t x, int32_t y, int32_t var4);
	static void drawString(shared_ptr<Font>& font, string var1, int32_t var2, int32_t var3, int32_t var4);
    void blit(int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5, int32_t var6);
};