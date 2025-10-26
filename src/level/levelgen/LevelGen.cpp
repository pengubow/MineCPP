#include <iostream>
#include <cmath>
#include "level/levelgen/LevelGen.h"
#include "Minecraft.h"

LevelGen::LevelGen(shared_ptr<Minecraft> minecraft)
    : minecraft(minecraft) {}


// thank you chatgpt for assigning variable names
shared_ptr<Level> LevelGen::generateLevel(string name, int32_t width, int32_t height, int32_t depth) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return nullptr;
    }

    minecraft->beginLevelLoading("Generating level");
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->blocks = vector<uint8_t>(width * height * depth);
    minecraft->levelLoadUpdate("Raising..");
    shared_ptr<Distort> mountainNoise = make_shared<Distort>(make_shared<PerlinNoise>(random, 8), make_shared<PerlinNoise>(random, 8));
    shared_ptr<Distort> hillNoise = make_shared<Distort>(make_shared<PerlinNoise>(random, 8), make_shared<PerlinNoise>(random, 8));
    shared_ptr<PerlinNoise> scaleNoise = make_shared<PerlinNoise>(random, 8);
    vector<int32_t> heightMap = vector<int32_t>(this->width * this->height);
    float noiseScale = 1.3f;

    int32_t x;
    int32_t z;
    for (x = 0; x < this->width; ++x) {
        this->setNextPhase(x * 100 / (this->width - 1));

        for (z = 0; z < this->height; ++z) {
            double mountainHeight = mountainNoise->getValue((double)((float)x * noiseScale), (double)((float)z * noiseScale)) / 8.0 - 8.0;
            double hillHeight = hillNoise->getValue((double)((float)x * noiseScale), (double)((float)z * noiseScale)) / 6.0 + 6.0;
            double scale = scaleNoise->getValue((double)x, (double)z) / 8.0;
            if (scale > 0.0) {
                hillHeight = mountainHeight;
            }

            double finalHeight = max(mountainHeight, hillHeight) / 2.0;
            if (finalHeight < 0.0) {
                finalHeight *= 0.8;
            }

            heightMap[x + z * this->width] = (int32_t)finalHeight;
        }
    }

    minecraft->levelLoadUpdate("Eroding..");
    vector<int32_t>& heightMapRef = heightMap;
    hillNoise = make_shared<Distort>(make_shared<PerlinNoise>(random, 8), make_shared<PerlinNoise>(random, 8));
    shared_ptr<Distort> erosionNoise = make_shared<Distort>(make_shared<PerlinNoise>(random, 8), make_shared<PerlinNoise>(random, 8));

    int32_t y;
    int32_t erosionValue;
    int32_t erosionCheck;
    for (x = 0; x < this->width; ++x) {
        this->setNextPhase(x * 100 / (this->width - 1));

        for (z = 0; z < this->height; ++z) {
            double erosionStrength = hillNoise->getValue((double)(x << 1), (double)(z << 1)) / 8.0;
            erosionCheck = erosionNoise->getValue((double)(x << 1), (double)(z << 1)) > 0.0 ? 1 : 0;
            if (erosionStrength > 2.0) {
                y = heightMapRef[x + z * this->width];
                y = ((y - erosionCheck) / 2 << 1) + erosionCheck;
                heightMapRef[x + z * this->width] = y;
            }
        }
    }

    minecraft->levelLoadUpdate("Soiling..");
    heightMapRef = heightMap;
    int32_t levelWidth = this->width;
    int32_t levelHeight = this->height;
    int32_t levelDepth = this->depth;
    shared_ptr<PerlinNoise> soilNoise = make_shared<PerlinNoise>(random, 8);

    int32_t dirtHeight;
    int32_t rockHeight;
    for (x = 0; x < levelWidth; ++x) {
        this->setNextPhase(x * 100 / (this->width - 1));

        for (z = 0; z < levelHeight; ++z) {
            erosionCheck = (int32_t)(soilNoise->getValue((double)x, (double)z) / 24.0) - 4;
            y = heightMapRef[x + z * levelWidth] + levelDepth / 2;
            rockHeight = y + erosionCheck;
            heightMapRef[x + z * levelWidth] = max(y, rockHeight);

            for(dirtHeight = 0; dirtHeight < levelDepth; ++dirtHeight) {
                int32_t blockIndex = (dirtHeight * this->height + z) * this->width + x;
                int32_t blockId = 0;
                if(dirtHeight <= y) {
                    blockId = Tile::dirt->id;
                }

                if(dirtHeight <= rockHeight) {
                    blockId = Tile::rock->id;
                }

                this->blocks[blockIndex] = (uint8_t)blockId;
            }
        }
    }

    minecraft->levelLoadUpdate("Carving..");
    int32_t caveCount = this->width * this->height * this->depth / 256 / 64;

    for (z = 0; z < caveCount; ++z) {
        this->setNextPhase(z * 100 / (caveCount - 1) / 4);
        float caveX = random.nextFloat() * (float)this->width;
        float caveY = random.nextFloat() * (float)this->depth;
        float caveZ = random.nextFloat() * (float)this->height;
        int32_t caveLength = (int32_t)((random.nextFloat() + random.nextFloat()) * 75.0f);
        float pitchAngle = (float)((double)random.nextFloat() * M_PI * 2.0);
        float pitchVelocity = 0.0F;
        float yawAngle = (float)((double)random.nextFloat() * M_PI * 2.0);
        float yawVelocity = 0.0F;

        for (int32_t caveStep = 0; caveStep < caveLength; ++caveStep) {
            caveX = (float)((double)caveX + sin((double)pitchAngle) * cos((double)yawAngle));
            caveZ = (float)((double)caveZ + cos((double)pitchAngle) * cos((double)yawAngle));
            caveY = (float)((double)caveY + sin((double)yawAngle));
            pitchAngle += pitchVelocity * 0.2F;
            pitchVelocity *= 0.9F;
            pitchVelocity += random.nextFloat() - random.nextFloat();
            yawAngle += yawVelocity * 0.5F;
            yawAngle *= 0.5F;
            yawVelocity *= 0.9F;
            yawVelocity += random.nextFloat() - random.nextFloat();
            
            if (random.nextFloat() >= 0.3F) {
                float caveRadiusX = caveX + random.nextFloat() * 4.0f - 2.0f;
                float caveRadiusY = caveY + random.nextFloat() * 4.0f - 2.0f;
                float caveRadiusZ = caveZ + random.nextFloat() * 4.0f - 2.0f;
                float caveRadius = (float)(sin((double)caveStep * M_PI / (double)caveLength) * 2.5 + 1.0);

                for (int32_t ix = (int32_t)(caveRadiusX - caveRadius); ix <= (int32_t)(caveRadiusX + caveRadius); ++ix) {
                    for (int32_t iy = (int32_t)(caveRadiusY - caveRadius); iy <= (int32_t)(caveRadiusY + caveRadius); ++iy) {
                        for (int32_t iz = (int32_t)(caveRadiusZ - caveRadius); iz <= (int32_t)(caveRadiusZ + caveRadius); ++iz) {
                            float dx = (float)ix - caveRadiusX;
                            float dy = (float)iy - caveRadiusY;
                            float dz = (float)iz - caveRadiusZ;
                            float distSq = dx * dx + dy * dy * 2.0F + dz * dz;
                            if (distSq < caveRadius * caveRadius && ix >= 1 && iy >= 1 && iz >= 1 && ix < this->width - 1 && iy < this->depth - 1 && iz < this->height - 1) {
                                int32_t blockIdx = (iy * this->height + iz) * this->width + ix;
                                if (this->blocks[blockIdx] == Tile::rock->id) {
                                    this->blocks[blockIdx] = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    this->carveTunnels(Tile::oreCoal->id, 90, 1, 4);
    this->carveTunnels(Tile::oreIron->id, 70, 2, 4);
    this->carveTunnels(Tile::oreGold->id, 50, 3, 4);
    minecraft->levelLoadUpdate("Watering..");
    int64_t startTime = Timer::nanoTime();
    int64_t filledBlocks = 0;
    int32_t waterTileId = Tile::calmWater->id;
    this->setNextPhase(0);

    for (z = 0; z < this->width; ++z) {
        filledBlocks += this->floodFillLiquid(z, this->depth / 2 - 1, 0, 0, waterTileId);
        filledBlocks += this->floodFillLiquid(z, this->depth / 2 - 1, this->height - 1, 0, waterTileId);
    }

    for (z = 0; z < this->height; ++z) {
        filledBlocks += this->floodFillLiquid(0, this->depth / 2 - 1, z, 0, waterTileId);
        filledBlocks += this->floodFillLiquid(this->width - 1, this->depth / 2 - 1, z, 0, waterTileId);
    }

    int32_t waterSpots = this->width * this->height / 200;

    for (int32_t i = 0; i < waterSpots; ++i) {
        if(i % 100 == 0) {
            this->setNextPhase(i * 100 / (waterSpots - 1));
        }

        int32_t spotX = random.nextInt(this->width);
        int32_t spotY = this->depth / 2 - 1 - random.nextInt(3);
        int32_t spotZ = random.nextInt(this->height);
        if (this->blocks[(spotY * this->height + spotZ) * this->width + spotX] == 0) {
            filledBlocks += this->floodFillLiquid(spotX, spotY, spotZ, 0, waterTileId);
        }
    }

    this->setNextPhase(100);
    int64_t endTime = Timer::nanoTime();
    cout << "Flood filled " + to_string(filledBlocks) + " tiles in " + to_string((double)(endTime - startTime) / 1000000.0) + " ms" << endl;
    minecraft->levelLoadUpdate("Melting..");
    this->addLava();
    minecraft->levelLoadUpdate("Growing..");
    this->addBeaches(heightMap);
    minecraft->levelLoadUpdate("Planting..");
    this->plantTrees(heightMap);
    shared_ptr<Level> level = make_shared<Level>();
    level->setData(width, 64, height, this->blocks);
    level->createTime = Timer::nanoTime() / 1000000;
    level->creator = name;
    level->name = "A Nice World";
    return level;
}

void LevelGen::addBeaches(vector<int32_t>& heightMap) {
    int32_t levelWidth = this->width;
    int32_t levelHeight = this->height;
    int32_t levelDepth = this->depth;
    shared_ptr<PerlinNoise> beachNoise1 = make_shared<PerlinNoise>(random, 8);
    shared_ptr<PerlinNoise> beachNoise2 = make_shared<PerlinNoise>(random, 8);

    for (int32_t x = 0; x < levelWidth; ++x) {
        setNextPhase(x * 100 / (this->width - 1));

        for (int32_t z = 0; z < levelHeight; ++z) {
            bool isSandBeach = beachNoise1->getValue((double)x, (double)z) > 8.0;
            bool isGravelBeach = beachNoise2->getValue((double)x, (double)z) > 12.0;
            int32_t surfaceHeight = heightMap[x + z * levelWidth];
            int32_t surfaceBlockIdx = (surfaceHeight * levelHeight + z) * this->width + x;
            int32_t aboveBlockId = blocks[((surfaceHeight + 1) * levelHeight + z) * this->width + x] & 255;
            
            if ((aboveBlockId == Tile::water->id || aboveBlockId == Tile::calmWater->id) && surfaceHeight <= levelDepth / 2 - 1 && isGravelBeach) {
                blocks[surfaceBlockIdx] = (uint8_t)Tile::gravel->id;
            }
            
            if (aboveBlockId == 0) {
                int32_t topBlockId = Tile::grass->id;
                if (surfaceHeight <= levelDepth / 2 - 1 && isSandBeach) {
                    topBlockId = Tile::sand->id;
                }

                blocks[surfaceBlockIdx] = (uint8_t)topBlockId;
            }
        }
    }
}

void LevelGen::plantTrees(vector<int32_t>& heightMap) {
    int32_t levelWidth = this->width;
    int32_t treeCount = this->width * this->height / 4000;

    for (int32_t treeIdx = 0; treeIdx < treeCount; ++treeIdx) {
        setNextPhase(treeIdx * 100 / (treeCount - 1));
        int32_t startX = random.nextInt(this->width);
        int32_t startZ = random.nextInt(this->height);

        for (int32_t attempt = 0; attempt < 20; ++attempt) {
            int32_t searchX = startX;
            int32_t searchZ = startZ;

            for (int32_t step = 0; step < 20; ++step) {
                searchX += random.nextInt(6) - random.nextInt(6);
                searchZ += random.nextInt(6) - random.nextInt(6);
                if (searchX >= 0 && searchZ >= 0 && searchX < this->width && searchZ < this->height) {
                    int32_t trunkBaseHeight = heightMap[searchX + searchZ * levelWidth] + 1;
                    int32_t trunkLength = random.nextInt(3) + 4;
                    bool canPlaceTree = true;

                    int32_t checkY;
                    int32_t checkX;
                    int32_t checkZ;
                    int32_t blockType;
                    for (checkY = trunkBaseHeight; checkY <= trunkBaseHeight + 1 + trunkLength; ++checkY) {
                        uint8_t checkRadius = 1;
                        if (checkY >= trunkBaseHeight + 1 + trunkLength - 2) {
                            checkRadius = 2;
                        }

                        for (checkX = searchX - checkRadius; checkX <= searchX + checkRadius && canPlaceTree; ++checkX) {
                            for (checkZ = searchZ - checkRadius; checkZ <= searchZ + checkRadius && canPlaceTree; ++checkZ) {
                                if (checkX >= 0 && checkY >= 0 && checkZ >= 0 && checkX < this->width && checkY < this->depth && checkZ < this->height) {
                                    blockType = blocks[(checkY * this->height + checkZ) * this->width + checkX] & 255;
                                    if (blockType != 0) {
                                        canPlaceTree = false;
                                    }
                                }
                                else {
                                    canPlaceTree = false;
                                }
                            }
                        }
                    }

                    if (canPlaceTree) {
                        checkY = (trunkBaseHeight * this->height + searchZ) * this->width + searchX;
                        int32_t groundBlockId = this->blocks[((trunkBaseHeight - 1) * this->height + searchZ) * this->width + searchX] & 255;
                        if (groundBlockId == Tile::grass->id && trunkBaseHeight < this->depth - trunkLength - 1) {
                            blocks[checkY - 1 * this->width * this->height] = (uint8_t)Tile::dirt->id;

                            for (checkX = trunkBaseHeight - 3 + trunkLength; checkX <= trunkBaseHeight + trunkLength; ++checkX) {
                                checkZ = checkX - (trunkBaseHeight + trunkLength);
                                blockType = 1 - checkZ / 2;

                                for (int32_t leafX = searchX - blockType; leafX <= searchX + blockType; ++leafX) {
                                    int32_t relX = leafX - searchX;

                                    for(int32_t leafZ = searchZ - blockType; leafZ <= searchZ + blockType; ++leafZ) {
                                        int32_t relZ = leafZ - searchZ;
                                        if(abs(relX) != blockType || abs(relZ) != blockType || random.nextInt(2) != 0 && checkZ != 0) {
                                            blocks[(checkX * this->height + leafZ) * this->width + leafX] = (uint8_t)Tile::leaf->id;
                                        }
                                    }
                                }
                            }

                            for (checkX = 0; checkX < trunkLength; ++checkX) {
                                this->blocks[checkY + checkX * this->width * this->height] = (uint8_t)Tile::log->id;
                            }
                        }
                    }
                }
            }
        }
    }
}

void LevelGen::carveTunnels(int32_t oreId, int32_t frequency, int32_t index, int32_t maxIndex) {
    uint8_t oreBlockId = (uint8_t)oreId;
    int32_t levelWidth = this->width;
    int32_t levelHeight = this->height;
    int32_t levelDepth = this->depth;
    int32_t tunnelCount = levelWidth * levelHeight * levelDepth / 256 / 64 * frequency / 100;

    for (int32_t i = 0; i < tunnelCount; i++) {
        this->setNextPhase(i * 100 / (tunnelCount - 1) / 4 + index * 100 / maxIndex);
        float tunnelX = random.nextFloat() * (float)levelWidth;
        float tunnelY = random.nextFloat() * (float)levelDepth;
        float tunnelZ = random.nextFloat() * (float)levelHeight;
        int tunnelLength = (int32_t)((random.nextFloat() + random.nextFloat()) * 75.0F * (float)frequency / 100.0F);
        float pitchAngle = (float)((double)random.nextFloat() * M_PI * 2.0);
        float pitchVelocity = 0.0F;
        float yawAngle = (float)((double)random.nextFloat() * M_PI * 2.0);
        float yawVelocity = 0.0F;

        for (int32_t step = 0; step < tunnelLength; step++) {
            tunnelX = (float)((double)tunnelX + sin((double)pitchAngle) * cos((double)yawAngle));
            tunnelZ = (float)((double)tunnelZ + cos((double)pitchAngle) * cos((double)yawAngle));
            tunnelY = (float)((double)tunnelY + sin((double)yawAngle));
            pitchAngle += pitchVelocity * 0.2F;
            pitchVelocity *= 0.9F;
            pitchVelocity += random.nextFloat() - random.nextFloat();
            yawAngle += yawVelocity * 0.5F;
            yawAngle *= 0.5F;
            yawVelocity *= 0.9F;
            yawVelocity += random.nextFloat() - random.nextFloat();
            float radius = (float)(sin((double)step * M_PI / (double)tunnelLength) * (double)frequency / 100.0 + 1.0);

            for (int32_t ix = (int32_t)(tunnelX - radius); ix <= (int32_t)(tunnelX + radius); ix++) {
                for (int32_t iy = (int32_t)(tunnelY - radius); iy <= (int32_t)(tunnelY + radius); iy++) {
                    for (int32_t iz = (int32_t)(tunnelZ - radius); iz <= (int32_t)(tunnelZ + radius); iz++) {
                        float dx = (float)ix - tunnelX;
                        float dy = (float)iy - tunnelY;
                        float dz = (float)iz - tunnelZ;
                        float distSq = dx * dx + dy * dy * 2.0F + dz * dz;
                        if (distSq < radius * radius && ix >= 1 && iy >= 1 && iz >= 1 && ix < this->width - 1 && iy < this->depth - 1 && iz < this->height - 1) {
                            int32_t blockIdx = (iy * this->height + iz) * this->width + ix;
                            if (this->blocks[blockIdx] == Tile::rock->id) {
                                this->blocks[blockIdx] = oreBlockId;
                            }
                        }
                    }
                }
            }
        }
    }
}

void LevelGen::setNextPhase(int32_t phase) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }

    minecraft->setLoadingProgress(phase);
}

void LevelGen::addLava() {
    int32_t lavaCount = 0;
    int32_t lavaSpots = this->width * this->height * this->depth / 10000;

    for (int32_t i = 0; i < lavaSpots; ++i) {
        if (i % 100 == 0) {
            this->setNextPhase(i * 100 / (lavaSpots - 1));
        }

        int32_t spotX = random.nextInt(this->width);
        int32_t spotY = random.nextInt(this->depth / 2 - 4);
        int32_t spotZ = random.nextInt(this->height);
        if (this->blocks[(spotY * this->height + spotZ) * this->width + spotX] == 0) {
            lavaCount++;
            this->floodFillLiquid(spotX, spotY, spotZ, 0, Tile::calmLava->id);
        }
    }

    this->setNextPhase(100);
    cout << "LavaCount: " + to_string(lavaCount) << endl;
}

int64_t LevelGen::floodFillLiquid(int32_t x, int32_t y, int32_t z, int32_t var4, int32_t liquidId) {
    uint8_t liquidBlockId = (uint8_t)liquidId;
    vector<vector<int32_t>> stackBackup = vector<vector<int32_t>>();
    uint8_t stackPtr = 0;
    int32_t xBits = 1;

    int32_t zBits;
    for (zBits = 1; 1 << xBits < this->width; ++xBits) {
    }

    while (1 << zBits < this->height) {
        zBits++;
    }

    int32_t yMask = this->height - 1;
    int32_t xMask = this->width - 1;
    int32_t stackSize = stackPtr + 1;
    this->coords[0] = ((y << zBits) + z << xBits) + x;
    int64_t filledCount = 0L;
    x = this->width * this->height;

    while (stackSize > 0) {
        stackSize--;
        y = this->coords[stackSize];
        if (stackSize == 0 && stackBackup.size() > 0) {
            cout << "IT HAPPENED!" << endl;
            this->coords = move(stackBackup.back());
            stackBackup.pop_back();
            stackSize = this->coords.size();
        }

        z = y >> xBits & yMask;
        int32_t yCoord = y >> xBits + zBits;
        int32_t xCoord = y & xMask;

        int32_t endX;
        for (endX = xCoord; xCoord > 0 && this->blocks[y - 1] == 0; y--) {
            xCoord--;
        }

        while (endX < this->width && this->blocks[y + endX - xCoord] == 0) {
            endX++;
        }

        int32_t checkZ = y >> xBits & yMask;
        int32_t checkY = y >> xBits + zBits;
        if(checkZ != z || checkY != yCoord) {
            cout << "hoooly fuck";
        }

        bool aboveEmpty = false;
        bool belowEmpty = false;
        bool upstairEmpty = false;
        filledCount += (int64_t)(endX - xCoord);

        for (xCoord = xCoord; xCoord < endX; ++xCoord) {
            this->blocks[y] = liquidBlockId;
            bool isEmpty;
            if (z > 0) {
                isEmpty = this->blocks[y - this->width] == 0;
                if (isEmpty && !aboveEmpty) {
                    if (stackSize == this->coords.size()) {
                        stackBackup.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        stackSize = 0;
                    }

                    this->coords[stackSize++] = y - this->width;
                }

                aboveEmpty = isEmpty;
            }

            if (z < this->height - 1) {
                isEmpty = this->blocks[y + this->width] == 0;
                if (isEmpty && !belowEmpty) {
                    if (stackSize == this->coords.size()) {
                        stackBackup.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        stackSize = 0;
                    }

                    this->coords[stackSize++] = y + this->width;
                }

                belowEmpty = isEmpty;
            }

            if (yCoord > 0) {
                uint8_t upBlock = this->blocks[y - x];
                if ((liquidBlockId == Tile::lava->id || liquidBlockId == Tile::calmLava->id) && (upBlock == Tile::water->id || upBlock == Tile::calmWater->id)) {
                    this->blocks[y - x] = (uint8_t)Tile::rock->id;
                }

                isEmpty = upBlock == 0;
                if (isEmpty && !upstairEmpty) {
                    if (stackSize == this->coords.size()) {
                        stackBackup.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        stackSize = 0;
                    }

                    this->coords[stackSize++] = y - x;
                }

                upstairEmpty = isEmpty;
            }

            ++y;
        }
    }

    return filledCount;
}