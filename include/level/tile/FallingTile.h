#include "level/tile/Tile.h"
#include "level/Level.h"

class FallingTile : public Tile {
public:
    FallingTile(int32_t id, int32_t tex);

    void onBlockAdded(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    void neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) override;
private:
    static void tryToFall(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z);
};