#include "level/tile/Tile.h"

class LeafTile : public Tile {
public:
    LeafTile(int32_t id, int32_t tex);

    bool isSolid() override;
    bool blocksLight() override;
};