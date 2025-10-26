#include "renderer/texture/TextureWaterFX.h"
#include "level/tile/Tile.h"

TextureWaterFX::TextureWaterFX()
    : TextureFX(Tile::water->tex) {}

void TextureWaterFX::onTick() {
    tickCounter++;

    for (int32_t var1 = 0; var1 < 16; ++var1) {
        for (int32_t var2 = 0; var2 < 16; ++var2) {
            float var3 = 0.0F;

            for (int var4 = var1 - 1; var4 <= var1 + 1; ++var4) {
                int var5 = var4 & 15;
                int var6 = var2 & 15;
                var3 += red[var5 + (var6 << 4)];
            }

            green[var1 + (var2 << 4)] = var3 / 3.3f + blue[var1 + (var2 << 4)] * 0.8f;
        }
    }

    for (int32_t var1 = 0; var1 < 16; ++var1) {
        for (int32_t var2 = 0; var2 < 16; ++var2) {
            blue[var1 + (var2 << 4)] += alpha[var1 + (var2 << 4)] * 0.05f;
            if (blue[var1 + (var2 << 4)] < 0.0f) {
                blue[var1 + (var2 << 4)] = 0.0f;
            }

            alpha[var1 + (var2 << 4)] -= 0.1f;
            if (Random::random() < 0.05) {
                alpha[var1 + (var2 << 4)] = 0.5f;
            }
        }
    }

    swap(green, red);

    for (int32_t var2 = 0; var2 < 256; ++var2) {
        float var3 = red[var2];
        if (var3 > 1.0f) {
            var3 = 1.0f;
        }

        if (var3 < 0.0f) {
            var3 = 0.0f;
        }

        float var8 = var3 * var3;
        imageData[var2 << 2] = (uint8_t)(32.0f + var8 * 32.0f);
        imageData[(var2 << 2) + 1] = (uint8_t)(50.0f + var8 * 64.0f);
        imageData[(var2 << 2) + 2] = -1;
        imageData[(var2 << 2) + 3] = (uint8_t)(146.0f + var8 * 50.0f);
    }

}