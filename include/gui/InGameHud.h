#include <deque>
#include "ChatLine.h"
#include "gui/Font.h"
#include "gui/ChatScreen.h"
#include "net/NetworkPlayer.h"
#include "Minecraft.h"

class InGameHud {
public:
    deque<ChatLine> messages;
private:
	weak_ptr<Minecraft> minecraft;
	int32_t scaledWidth;
	int32_t scaledHeight;
public:
	InGameHud(shared_ptr<Minecraft>& minecraft, int32_t scaledWidth, int32_t scaledHeight);

	void render();
private:
	static void blit(int32_t var0, int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5);
public:
	void addChatMessage(string message);
};