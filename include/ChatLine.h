#pragma once

#include <string>

using namespace std;

class ChatLine {
public:
    string message;
    int32_t counter;

    ChatLine(string message);
};