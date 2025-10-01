#include "HitResult.h"

HitResult::HitResult(int32_t type, int32_t x, int32_t y, int32_t z, int32_t face) {
    this->type = type;
    this->x = x;
    this->y = y;
    this->z = z;
    this->face = face;
}