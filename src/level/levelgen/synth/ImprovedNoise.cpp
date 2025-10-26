#include <iostream>
#include <cmath>
#include "level/levelgen/synth/ImprovedNoise.h"
#include "Util/Util.h"

ImprovedNoise::ImprovedNoise() {
    Random random = Random();
    p = vector<int32_t>(512);

    int32_t i;
    for(i = 0; i < 256; p[i] = i++) {}

    for(i = 0; i < 256; i++) {
        int32_t var3 = random.nextInt(256 - i) + i;
        int32_t var4 = p[i];
        p[i] = p[var3];
        p[var3] = var4;
        p[i + 256] = p[i];
    }
}

ImprovedNoise::ImprovedNoise(Random& random) {
    p = vector<int32_t>(512);

    int32_t i;
    for(i = 0; i < 256; p[i] = i++) {}

    for(i = 0; i < 256; i++) {
        int32_t var3 = random.nextInt(256 - i) + i;
        int32_t var4 = p[i];
        p[i] = p[var3];
        p[var3] = var4;
        p[i + 256] = p[i];
    }
}

double ImprovedNoise::fade(double var0) {
    return var0 * var0 * var0 * (var0 * (var0 * 6.0 - 15.0) + 10.0);
}

double ImprovedNoise::lerp(double var0, double var2, double var4) {
    return var2 + var0 * (var4 - var2);
}

double ImprovedNoise::grad(int32_t var0, double var1, double var3, double var5) {
    var0 &= 15;
    double var8 = var0 < 8 ? var1 : var3;
    double var10 = var0 < 4 ? var3 : (var0 != 12 && var0 != 14 ? var5 : var1);
    return ((var0 & 1) == 0 ? var8 : -var8) + ((var0 & 2) == 0 ? var10 : -var10);
}

double ImprovedNoise::getValue(double var1, double var3) {
    double var10 = 0.0;
    double var8 = var3;
    double var6 = var1;
    int32_t var21 = (int32_t)floor(var1) & 255;
    int32_t var2 = (int32_t)floor(var3) & 255;
    int32_t var22 = (int32_t)floor(0.0) & 255;
    var6 -= floor(var6);
    var8 -= floor(var8);
    var10 = 0.0 - floor(0.0);
    double var15 = fade(var6);
    double var17 = fade(var8);
    double var19 = fade(var10);
    int32_t var4 = p[var21] + var2;
    int32_t var5 = p[var4] + var22;
    var4 = p[var4 + 1] + var22;
    var21 = p[var21 + 1] + var2;
    var2 = p[var21] + var22;
    var21 = p[var21 + 1] + var22;
    return lerp(var19, lerp(var17, lerp(var15, grad(p[var5], var6, var8, var10), grad(p[var2], var6 - 1.0, var8, var10)), lerp(var15, grad(p[var4], var6, var8 - 1.0, var10), grad(p[var21], var6 - 1.0, var8 - 1.0, var10))), lerp(var17, lerp(var15, grad(p[var5 + 1], var6, var8, var10 - 1.0), grad(p[var2 + 1], var6 - 1.0, var8, var10 - 1.0)), lerp(var15, grad(p[var4 + 1], var6, var8 - 1.0, var10 - 1.0), grad(p[var21 + 1], var6 - 1.0, var8 - 1.0, var10 - 1.0))));
}