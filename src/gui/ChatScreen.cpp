#include <GLFW/glfw3.h>
#include "gui/ChatScreen.h"
#include "net/ConnectionManager.h"
#include "net/Packet.h"
#include "comm/SocketConnection.h"
#include "Util/Util.h"
#include "Minecraft.h"

void ChatScreen::init() {
    glfwSetInputMode(Util::getGLFWWindow(), GLFW_STICKY_KEYS, GLFW_TRUE);
}

void ChatScreen::closeScreen() {
    glfwSetInputMode(Util::getGLFWWindow(), GLFW_STICKY_KEYS, GLFW_FALSE);
}

void ChatScreen::tick() {
    counter++;
}

void ChatScreen::keyPressed(char var1, int32_t key) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        minecraft->setScreen(nullptr);
    }
    else if (key == GLFW_KEY_ENTER) {
        string var4 = Util::trim(TypedMsg);
        if (var4.length() > 0) {
            if (!minecraft->connectionManager) {
                throw runtime_error("On java, this caused NullPointerException");
            }
            minecraft->connectionManager->connection->sendPacket(Packet::CHAT_MESSAGE, {(int8_t)-1, var4});
        }

        minecraft->setScreen(nullptr);
    }
    else {
        if (key == GLFW_KEY_BACKSPACE && TypedMsg.length() > 0) {
            TypedMsg.pop_back();
        }

        string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,.:-_\'*!\\\"#%/()=+?[]{}<>@|$;";
        if (allowed.find(var1) != string::npos && TypedMsg.length() < 64 - (minecraft->user->name.length() + 2)) {
            TypedMsg += var1;
        }
    }
}

void ChatScreen::render(int32_t var1, int32_t var2) {
    fill(2, height - 14, width - 2, height - 2, INT32_MIN);
    drawString("> " + TypedMsg + (counter / 6 % 2 == 0 ? "_" : ""), 4, height - 12, 14737632);
}