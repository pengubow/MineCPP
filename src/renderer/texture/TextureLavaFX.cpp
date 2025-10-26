#include <cmath>
#include "renderer/texture/TextureLavaFX.h"
#include "level/tile/Tile.h"

TextureLavaFX::TextureLavaFX() 
    : TextureFX(Tile::lava->tex) {}

void TextureLavaFX::onTick() {
    for (int32_t i = 0; i < 16; i++) {
        for (int32_t i2 = 0; i2 < 16; i2++) {
            float var3 = 0.0f;
            int32_t var4 = (int32_t)(sin((double)i2 * M_PI * 2.0 / 16.0) * (double)1.2f);
            int32_t var5 = (int32_t)(sin((double)i * M_PI * 2.0 / 16.0) * (double)1.2f);

            for (int32_t i3 = i - 1; i3 <= i + 1; i3++) {
                for (int32_t i4 = i2 - 1; i4 <= i2 + 1; i4++) {
                    int var8 = i3 + var4 & 15;
                    int var9 = i4 + var5 & 15;
                    var3 += red[var8 + (var9 << 4)];
                }
            }

            green[i + (i2 << 4)] = var3 / 10.0f + (blue[(i & 15) + ((i2 & 15) << 4)] 
                + blue[(i + 1 & 15) + ((i2 & 15) << 4)] + blue[(i + 1 & 15) + ((i2 + 1 & 15) << 4)] 
                + blue[(i & 15) + ((i2 + 1 & 15) << 4)]) / 4.0f * 0.8f;
            blue[i + (i2 << 4)] += alpha[i + (i2 << 4)] * 0.01f;
            if (blue[i + (i2 << 4)] < 0.0f) {
                blue[i + (i2 << 4)] = 0.0f;
            }

            alpha[i + (i2 << 4)] -= 0.06f;
            if (Random::random() < 0.005) {
                alpha[i + (i2 << 4)] = 1.5f;
            }
        }
    }

    swap(green, red);

    for (int32_t i = 0; i < 256; i++) {
        float var3 = red[i];
        if (var3 > 1.0f) {
            var3 = 1.0f;
        }

        if (var3 < 0.0f) {
            var3 = 0.0f;
        }

        imageData[i << 2] = (uint8_t)(var3 * 200.0f + 55.0f);
        imageData[(i << 2) + 1] = (uint8_t)(var3 * var3 * 255.0f);
        imageData[(i << 2) + 2] = (uint8_t)(var3 * var3 * var3 * var3 * 128.0f);
        imageData[(i << 2) + 3] = -1;
    }
}