#include "HitResult.h"

HitResult::HitResult(int32_t var1, int32_t x, int32_t y, int32_t z, int32_t face) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->face = face;
}

float HitResult::distanceTo(Player* player, int32_t editMode) {
    int32_t x = this->x;
    int32_t y = this->y;
    int32_t z = this->z;
    if(editMode == 1) {
        if(face == 0) {
            y--;
        }

        if(face == 1) {
            y++;
        }

        if(face == 2) {
            z--;
        }

        if(face == 3) {
            z++;
        }

        if(face == 4) {
            x--;
        }

        if(face == 5) {
            x++;
        }
    }

    float var6 = (float)x - player->x;
    float var8 = (float)y - player->y;
    float var7 = (float)z - player->z;
    return var6 * var6 + var8 * var8 + var7 * var7;
}