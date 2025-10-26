#include "renderer/texture/TextureFX.h"

class TextureWaterFX : public TextureFX {
    vector<float> red = vector<float>(256);
    vector<float> green = vector<float>(256);
    vector<float> blue = vector<float>(256);
    vector<float> alpha = vector<float>(256);
    int32_t tickCounter = 0;
public:
    TextureWaterFX();

    void onTick() override;
};