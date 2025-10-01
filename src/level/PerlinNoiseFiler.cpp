#include "level/PerlinNoiseFilter.h"

PerlinNoiseFilter::PerlinNoiseFilter(int32_t levels) 
    : levels(levels) {}

std::vector<int32_t> PerlinNoiseFilter::read(int32_t width, int32_t height) {
    int32_t x;
    std::vector<int32_t> tmp = std::vector<int32_t>(width * height);
    int32_t level = this->levels;
    int32_t step = width >> level;
    int32_t y = 0;
    while (y < height) {
        x = 0;
        while (x < width) {
            tmp.at(x + y * width) = (Util::nextInt(256) - 128) * this->fuzz;
            x += step;
        }
        y += step;
    }
    step = width >> level;
    while (step > 1) {
        int32_t x2;
        int32_t val = 256 * (step << level);
        int32_t ss = step / 2;
        int32_t y2 = 0;
        while (y2 < height) {
            x2 = 0;
            while (x2 < width) {
                int32_t m;
                int32_t ul = tmp.at((x2 + 0) % width + (y2 + 0) % height * width);
                int32_t ur = tmp.at((x2 + step) % width + (y2 + 0) % height * width);
                int32_t dl = tmp.at((x2 + 0) % width + (y2 + step) % height * width);
                int32_t dr = tmp.at((x2 + step) % width + (y2 + step) % height * width);
                tmp.at(x2 + ss + (y2 + ss) * width) = m = (ul + dl + ur + dr) / 4 + Util::nextInt(val * 2) - val;
                x2 += step;
            }
            y2 += step;
        }
        y2 = 0;
        while (y2 < height) {
            x2 = 0;
            while (x2 < width) {
                int32_t c = tmp.at(x2 + y2 * width);
                int32_t r = tmp.at((x2 + step) % width + y2 * width);
                int32_t d = tmp.at(x2 + (y2 + step) % width * width);
                int32_t mu = tmp.at(((x2 + ss) & (width - 1)) + (y2 + ss - step & height - 1) * width);
                int32_t ml = tmp.at((x2 + ss - step & width - 1) + (y2 + ss & height - 1) * width);
                int32_t m = tmp.at((x2 + ss) % width + (y2 + ss) % height * width);
                int32_t u = (c + r + m + mu) / 4 + Util::nextInt(val * 2) - val;
                int32_t l = (c + d + m + ml) / 4 + Util::nextInt(val * 2) - val;
                tmp.at(x2 + ss + y2 * width) = u;
                tmp.at(x2 + (y2 + ss) * width) = l;
                x2 += step;
            }
            y2 += step;
        }
        step /= 2;
    }
    std::vector<int32_t> result = std::vector<int32_t>(width * height);
    y = 0;
    while (y < height) {
        x = 0;
        while (x < width) {
            result.at(x + y * width) = tmp.at(x % width + y % height * width) / 512 + 128;
            ++x;
        }
        ++y;
    }
    return result;
}