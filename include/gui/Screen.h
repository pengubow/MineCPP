#pragma once

#include "gui/Gui.h"
#include "gui/Button.h"

class Minecraft;

class Screen : public Gui {
protected:
	weak_ptr<Minecraft> minecraft;
	int32_t width;
	int32_t height;
public:
	bool allowUserInput = false;
    vector<shared_ptr<Button>> buttons;
protected:
	shared_ptr<Font> font;
public:

	virtual void render(int32_t var1, int32_t var2);
protected:
    virtual void keyPressed(char var1, int32_t key);
	virtual void mousePressed(int32_t x, int32_t y, int32_t clickType);
    virtual void buttonClicked(shared_ptr<Button>& button);
public:
	virtual void init(shared_ptr<Minecraft>& minecraft, int32_t width, int32_t height);
	virtual void init();
	void updateEvents();
	void updateMouseEvents();
	void updateKeyboardEvents();
	virtual void tick();
	virtual void closeScreen();
};