#include "level/tile/Tile.h"
#include "level/Level.h"

class SpongeTile : public Tile {
public:
    SpongeTile(int32_t id);

    void onTileAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    void onTileRemoved(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override; 
};