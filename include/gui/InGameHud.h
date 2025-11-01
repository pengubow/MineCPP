#include <deque>
#include "ChatLine.h"
#include "gui/Font.h"
#include "gui/ChatScreen.h"
#include "net/NetworkPlayer.h"
#include "Minecraft.h"

class InGameHud : public Gui {
public:
    deque<ChatLine> messages;
private:
	weak_ptr<Minecraft> minecraft;
	int32_t scaledWidth;
	int32_t scaledHeight;
public:
	string hoveredUsername;

	InGameHud(shared_ptr<Minecraft>& minecraft, int32_t scaledWidth, int32_t scaledHeight);

	void render(bool var1, int32_t var2, int32_t var3);
	void addChatMessage(string message);
};