#include "User.h"
#include "level/tile/Tile.h"

vector<int32_t>& User::getCreativeTiles() {
    static vector<int32_t> creativeTiles = {Tile::rock->id, Tile::dirt->id, Tile::sponge->id, Tile::wood->id, Tile::bush->id, Tile::log->id, Tile::leaf->id, Tile::glass->id, Tile::gravel->id};
    return creativeTiles;
}

User::User(string name, string sessionId) 
    : name(name), sessionId(sessionId) {}
