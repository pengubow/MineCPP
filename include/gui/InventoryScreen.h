#include "gui/Screen.h"

class InventoryScreen : public Screen {
    int32_t getTileAtSlot(int32_t var1, int32_t var2);
public:
    void render(int32_t var1, int32_t var2) override;
protected:
    void mousePressed(int32_t x, int32_t y, int32_t clickType) override;
};