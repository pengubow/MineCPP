#include "gui/Screen.h"

class ErrorScreen : public Screen {
    string title;
    string desc;
public:
    ErrorScreen(string title, string desc);

    void init() override;
    void render(int32_t var1, int32_t var2) override;
protected:
    void keyPressed(char var1, int32_t key) override;
};