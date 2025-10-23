#include "gui/Screen.h"

class NameLevelScreen : public Screen {
    shared_ptr<Screen> parent;
    string title = "Enter level name:";
    int32_t id;
    string name;
    int32_t counter = 0;
public:
    NameLevelScreen(shared_ptr<Screen> parent, string name, int32_t id);

    void init() override;
    void closeScreen() override;
    void tick() override;
protected:
    void buttonClicked(shared_ptr<Button>& button) override;
    void keyPressed(char var1, int32_t key) override;
public:
    void render(int32_t var1, int32_t var2) override;
};