#include "gui/Screen.h"

class PauseScreen : public Screen, public enable_shared_from_this<PauseScreen> {
public:
	void init() override;
	void buttonClicked(shared_ptr<Button>& button) override;
public:
	void render(int32_t var1, int32_t var2) override;
};