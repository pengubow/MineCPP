#include "User.h"
#include "level/tile/Tile.h"

class Inventory {
public:
    vector<int32_t> slots = vector<int32_t>(9);
    int32_t selectedSlot = 0;

    Inventory();

    int32_t getSelected();
    int32_t getSlotContainsID(int32_t id);
    void scrollHotbar(int32_t var1);
    void getSlotContainsTile(Tile* tile);
};