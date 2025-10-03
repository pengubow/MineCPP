#include "level/LevelGen.h"
#include "level/NoiseMap.h"
#include "level/tile/Tile.h"
#include "Util.h"

using namespace std;

LevelGen::LevelGen(int32_t width, int32_t height, int32_t depth) {
    this->width = width;
    this->height = height;
    this->depth = depth;
}

vector<uint8_t> LevelGen::generateMap() {
    int32_t width = this->width;
    int32_t height = this->height;
    int32_t depth = this->depth;
    vector<int32_t> heightmap1 = NoiseMap(0).read(width, height);
    vector<int32_t> heightmap2 = NoiseMap(0).read(width, height);
    vector<int32_t> cf = NoiseMap(1).read(width, height);
    vector<int32_t> rockMap = NoiseMap(1).read(width, height);
    vector<uint8_t> blocks = vector<uint8_t>(this->width * this->height * this->depth);
    
    for (int32_t x = 0; x < width; x++) {
        for (int32_t y = 0; y < depth; y++) {
            for (int32_t z = 0; z < height; z++) {
                int32_t dh;
                int32_t dh1 = heightmap1[x + z * this->width];
                int32_t dh2 = heightmap2[x + z * this->width];
                int32_t cfh = cf[x + z * this->width];
                if (cfh < 128) {
                    dh2 = dh1;
                }
                if (dh2 > (dh = dh1)) {
                    dh = dh2;
                } else {
                    dh2 = dh1;
                }
                dh = dh / 8 + depth / 3;
                int32_t rh = rockMap[x + z * this->width] / 8 + depth / 3;
                if (rh > dh - 2) {
                    rh = dh - 2;
                }
                int32_t i = (y * this->height + z) * this->width + x;
                int32_t id = 0;
                if (y == dh) {
                    id = Tile::grass->id;
                }
                if (y < dh) {
                    id = Tile::dirt->id;
                }
                if (y <= rh) {
                    id = Tile::rock->id;
                }
                blocks[i] = (uint8_t)id;
            }
        }
    }

    int32_t count = width * height * depth / 256 / 64;
    for (int32_t i = 0; i < count; i++) {
        float x2 = Util::nextFloat() * (float)width;
        float y = Util::nextFloat() * (float)depth;
        float z = Util::nextFloat() * (float)height;
        int32_t length = (int32_t)(Util::nextFloat() + Util::nextFloat() * 150.0f);
        float dir1 = (float)(Util::random() * M_PI * 2.0);
        float dira1 = 0.0f;
        float dir2 = (float)((double)Util::nextFloat() * M_PI * 2.0);
        float dira2 = 0.0f;
        for (int32_t l = 0; l < length; l++) {
            x2 = (float)((double)x2 + sin(dir1) * cos(dir2));
            z = (float)((double)z + cos(dir1) * cos(dir2));
            y = (float)((double)y + sin(dir2));
            dir1 += dira1 * 0.2f;
            dira1 *= 0.9f;
            dira1 += Util::nextFloat() - Util::nextFloat();
            dir2 += dira2 * 0.5f;
            dir2 *= 0.5f;
            dira2 *= 0.9f;
            dira2 += Util::nextFloat() - Util::nextFloat();
            float size = (float)(sin((double)l * M_PI / (double)length) * 2.5 + 1.0);
            for (int32_t xx = (int32_t)(x2 - size); xx <= (int32_t)(x2 + size); xx++) {
                for (int32_t yy = (int32_t)(y - size); yy <= (int32_t)(y + size); yy++) {
                    for (int32_t zz = (int32_t)(z - size); zz <= (int32_t)(z + size); zz++) {
                        int32_t ii;
                        float xd = (float)xx - x2;
                        float yd = (float)yy - y;
                        float zd = (float)zz - z;
                        float dd = xd * xd + yd * yd * 2.0f + zd * zd;
                        if (dd < size * size && xx >= 1 && yy >= 1 && zz >= 1 && xx < this->width - 1 && yy < this->depth - 1 && zz < this->height - 1 && blocks[ii = (yy * this->height + zz) * this->width + xx] == Tile::rock->id) {
                            blocks[ii] = 0;
                        }
                    }
                }
            }
        }
    }
    return blocks;
}