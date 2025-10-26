#include "level/tile/Tile.h"
#include "level/Level.h"

class GlassTile : public Tile {
    bool renderAdjacentFaces = false;
public:
    GlassTile(int32_t id, int32_t tex, bool var3);

    bool isSolid() override;
protected:
    bool shouldRenderFace(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t layer, int32_t var6) override;
public:
    bool blocksLight() override;
};