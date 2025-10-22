#include "gui/LoadLevelScreen.h"

class SaveLevelScreen : public LoadLevelScreen, public enable_shared_from_this<LoadLevelScreen> {
public:
    SaveLevelScreen(shared_ptr<Screen> parent);
protected:
    void setLevels(vector<string>& var1) override;
    void loadLevel(int32_t var1) override;
};