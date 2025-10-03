#include "level/Level.h"
#include "level/tile/Tile.h"

class GrassTile : public Tile {
public:
    GrassTile(int32_t id);
protected:
    int32_t getTexture(int32_t face);
public:
    void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z); 
};