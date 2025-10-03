#include "Util.h"

class NoiseMap {
    int32_t seed = Util::nextInt();
    int32_t levels = 0;
    int32_t fuzz = 16;
public:
    NoiseMap(int32_t levels);

    vector<int32_t> read(int32_t width, int32_t height);
};