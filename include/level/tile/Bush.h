#include <memory>
#include "level/Level.h"
#include "level/tile/Tile.h"

class Bush : public Tile {
public:
    Bush(int32_t id);

    void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override; 
    bool render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) override;
    optional<AABB> getAABB(int32_t x, int32_t y, int32_t z) override;
    bool blocksLight() override;
    bool isSolid() override;
};