#include "Util.h"

class PerlinNoiseFilter {
    int32_t seed = Util::nextInt();
    int32_t levels = 0;
    int32_t fuzz = 16;

public:
    PerlinNoiseFilter(int32_t levels);

    std::vector<int32_t> read(int32_t width, int32_t height);
};