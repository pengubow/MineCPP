#include "gui/Screen.h"

class NewLevelScreen : public Screen {
private:
    shared_ptr<Screen> parent;
public:
    NewLevelScreen(shared_ptr<Screen> parent);

    void init() override;
    void buttonClicked(shared_ptr<Button>& var1) override;
    void render(int32_t var1, int32_t var2) override;
};