#include "level/tile/LiquidTile.h"

class CalmLiquidTile : public LiquidTile {
public:
    CalmLiquidTile(int32_t id, int32_t tex);
protected:
    void tick(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z) override;
    void neighborChanged(shared_ptr<Level>& level, int32_t x, int32_t y, int32_t z, int32_t type) override;
};