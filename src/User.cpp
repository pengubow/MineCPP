#include "User.h"

vector<Tile*> User::getCreativeTiles() {
    vector<Tile*> creativeTiles;
    creativeTiles.push_back(Tile::rock);
    creativeTiles.push_back(Tile::wood);
    creativeTiles.push_back(Tile::dirt);
    creativeTiles.push_back(Tile::stoneBrick);
    creativeTiles.push_back(Tile::log);
    creativeTiles.push_back(Tile::leaf);
    creativeTiles.push_back(Tile::bush);
    creativeTiles.push_back(Tile::plantYellow);
    creativeTiles.push_back(Tile::plantRed);
    creativeTiles.push_back(Tile::mushroomBrown);
    creativeTiles.push_back(Tile::mushroomRed);
    creativeTiles.push_back(Tile::sand);
    creativeTiles.push_back(Tile::gravel);
    creativeTiles.push_back(Tile::glass);
    creativeTiles.push_back(Tile::sponge);
    creativeTiles.push_back(Tile::blockGold);
    creativeTiles.push_back(Tile::clothRed);
    creativeTiles.push_back(Tile::clothOrange);
    creativeTiles.push_back(Tile::clothYellow);
    creativeTiles.push_back(Tile::clothChartreuse);
    creativeTiles.push_back(Tile::clothGreen);
    creativeTiles.push_back(Tile::clothSpringGreen);
    creativeTiles.push_back(Tile::clothCyan);
    creativeTiles.push_back(Tile::clothCapri);
    creativeTiles.push_back(Tile::clothUltramarine);
    creativeTiles.push_back(Tile::clothViolet);
    creativeTiles.push_back(Tile::clothPurple);
    creativeTiles.push_back(Tile::clothMagenta);
    creativeTiles.push_back(Tile::clothRose);
    creativeTiles.push_back(Tile::clothDarkGray);
    creativeTiles.push_back(Tile::clothGray);
    creativeTiles.push_back(Tile::clothWhite);
    return creativeTiles;
}

User::User(string name, string sessionId) 
    : name(name), sessionId(sessionId) {}
