#include "player/Inventory.h"

Inventory::Inventory() {
    for (int32_t i = 0; i < 9; i++)  {
        slots[i] = User::getCreativeTiles()[i]->id;
    }
}

int32_t Inventory::getSelected() {
    return slots[selectedSlot];
}

int32_t Inventory::getSlotContainsID(int32_t id) {
    for (int32_t i = 0; i < slots.size(); i++) {
        if (id == slots[i]) {
            return i;
        }
    }
    return -1;
}

void Inventory::scrollHotbar(int32_t var1) {
    for (selectedSlot -= var1; selectedSlot < 0; selectedSlot += slots.size()) {}

    while (selectedSlot >= slots.size()) {
        selectedSlot -= slots.size();
    }
}

void Inventory::getSlotContainsTile(Tile* tile) {
    if (tile != nullptr) {
        int32_t slot = getSlotContainsID(tile->id);
        if (slot >= 0) {
            slots[slot] = slots[selectedSlot];
        }
        slots[selectedSlot] = tile->id;
    }
}