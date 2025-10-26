#include "renderer/texture/TextureFX.h"

class TextureLavaFX : public TextureFX {
    vector<float> red = vector<float>(256);
    vector<float> green = vector<float>(256);
    vector<float> blue = vector<float>(256);
    vector<float> alpha = vector<float>(256);
public:
    TextureLavaFX();

    void onTick() override;
};