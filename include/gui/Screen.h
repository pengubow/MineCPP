#pragma once

#include "renderer/Tesselator.h"
#include "gui/Button.h"

class Minecraft;

class Screen {
protected:
	shared_ptr<Minecraft> minecraft;
	int32_t width;
	int32_t height;
public:
    vector<shared_ptr<Button>> buttons;
	virtual void render(int32_t var1, int32_t var2);
protected:
    virtual void keyPressed(int32_t key);
    virtual void buttonClicked(shared_ptr<Button>& var1);
public:
	virtual void init(shared_ptr<Minecraft>& minecraft, int32_t width, int32_t height);
	virtual void init();
protected:
	static void fill(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4);
	static void fillGradient(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5);
public:
	void drawCenteredString(string var1, int32_t var2, int32_t var3, int32_t var4);
	void drawString(string var1, int32_t var2, int32_t var3, int32_t var4);
	void updateEvents();
	virtual void tick();
	virtual void closeScreen();
};