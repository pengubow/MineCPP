#include "level/tile/Tile.h"

class LogTile : public Tile {
public:
    LogTile(int32_t tex);
protected:
    int32_t getTexture(int32_t face) override;
};