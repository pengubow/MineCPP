#include <vector>
#include <stdint.h>

class LevelGen {
private:
    int32_t width;
    int32_t height;
    int32_t depth;
public:
    LevelGen(int32_t width, int32_t height, int32_t depth);
    std::vector<uint8_t> generateMap();
};