#include <stdint.h>

class Coord {
public:
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t id;
    int32_t scheduledTime;

    Coord(int32_t x, int32_t y, int32_t z, int32_t id);
};
