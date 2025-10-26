#pragma once

#include <string>
#include <vector>
#include "level/tile/Tile.h"

using namespace std;

class User {
public:
    string name;
    string sessionId;
    string mpPass;

    User(string name, string sessionId);

    static vector<Tile*> getCreativeTiles();
};
