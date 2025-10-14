#pragma once

#include <string>

using namespace std;

class User {
public:
    string name;
    string sessionId;

    User(string name, string sessionId);
};
