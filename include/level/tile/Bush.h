#include <memory>
#include "level/Level.h"
#include "level/tile/Tile.h"

class Bush : public Tile {
public:
    Bush(int32_t id);

public:
    void tick(std::shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override; 
    void render(shared_ptr<Tesselator>& t, shared_ptr<Level>& level, int32_t layer, int32_t x, int32_t y, int32_t z) override;
    std::shared_ptr<AABB> getAABB(int32_t x, int32_t y, int32_t z) override;
    bool blocksLight() override;
    bool isSolid() override;
};