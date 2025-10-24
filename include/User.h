#pragma once

#include <string>
#include <vector>

using namespace std;

class User {
public:
    string name;
    string sessionId;
    string mpPass;

    User(string name, string sessionId);

    static vector<int32_t>& getCreativeTiles();
};
