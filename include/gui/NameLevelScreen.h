#include "gui/Screen.h"

class NameLevelScreen : public Screen {
private:
    shared_ptr<Screen> parent;
    string title = "Enter level name:";
    int32_t id;
    string name;
    int32_t counter = 0;
public:
    NameLevelScreen(shared_ptr<Screen> parent, string name, int32_t id);
    void init() override;
protected:
    void keyPressed(char var1, int32_t key);
public:
    void render(int32_t var1, int32_t var2) override;
};