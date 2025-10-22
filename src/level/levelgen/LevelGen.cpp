#include <iostream>
#include <cmath>
#include "level/levelgen/LevelGen.h"
#include "Minecraft.h"

LevelGen::LevelGen(shared_ptr<Minecraft> minecraft)
    : minecraft(minecraft) {}

shared_ptr<Level> LevelGen::generateLevel(string name, int32_t width, int32_t height, int32_t depth) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return nullptr;
    }

    minecraft->beginLevelLoading("Generating level");
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->blocks = vector<uint8_t>(width * height << 6);
    minecraft->levelLoadUpdate("Raising..");
    shared_ptr<Distort> var8 = make_shared<Distort>(make_shared<PerlinNoise>(8), make_shared<PerlinNoise>(8));
    shared_ptr<Distort> var9 = make_shared<Distort>(make_shared<PerlinNoise>(8), make_shared<PerlinNoise>(8));
    shared_ptr<PerlinNoise> var10 = make_shared<PerlinNoise>(8);
    vector<int32_t> var11 = vector<int32_t>(this->width * this->height);
    float var6 = 1.3f;

    int32_t var13;
    int32_t var14;
    for (var13 = 0; var13 < this->width; ++var13) {
        this->setNextPhase(var13 * 100 / (this->width - 1));

        for (var14 = 0; var14 < this->height; ++var14) {
            double var15 = var8->getValue((double)((float)var13 * var6), (double)((float)var14 * var6)) / 8.0 - 8.0;
            double var17 = var9->getValue((double)((float)var13 * var6), (double)((float)var14 * var6)) / 6.0 + 6.0;
            double var19 = var10->getValue((double)var13, (double)var14) / 8.0;
            if (var19 > 0.0) {
                var17 = var15;
            }

            double var21 = max(var15, var17) / 2.0;
            if (var21 < 0.0) {
                var21 /= 2.0;
            }

            var11[var13 + var14 * this->width] = (int32_t)var21;
        }
    }

    minecraft->levelLoadUpdate("Eroding..");
    vector<int32_t>& var31 = var11;
    var9 = make_shared<Distort>(make_shared<PerlinNoise>(8), make_shared<PerlinNoise>(8));
    shared_ptr<Distort> var37 = make_shared<Distort>(make_shared<PerlinNoise>(8), make_shared<PerlinNoise>(8));

    int32_t var16;
    int32_t var29;
    int32_t var39;
    int32_t var42;
    for (var39 = 0; var39 < this->width; ++var39) {
        this->setNextPhase(var39 * 100 / (this->width - 1));

        for (var29 = 0; var29 < this->height; ++var29) {
            double var41 = var9->getValue((double)(var39 << 1), (double)(var29 << 1)) / 8.0;
            var42 = var37->getValue((double)(var39 << 1), (double)(var29 << 1)) > 0.0 ? 1 : 0;
            if (var41 > 2.0) {
                var16 = var31[var39 + var29 * this->width];
                var16 = ((var16 - var42) / 2 << 1) + var42;
                var31[var39 + var29 * this->width] = var16;
            }
        }
    }

    minecraft->levelLoadUpdate("Soiling..");
    var31 = var11;
    int32_t var34 = this->width;
    int32_t var38 = this->height;
    var39 = this->depth;
    shared_ptr<PerlinNoise> var30 = make_shared<PerlinNoise>(8);

    int32_t var18;
    int32_t var46;
    for (var13 = 0; var13 < var34; ++var13) {
        this->setNextPhase(var13 * 100 / (this->width - 1));

        for (var14 = 0; var14 < var38; ++var14) {
            var42 = (int32_t)(var30->getValue((double)var13, (double)var14) / 24.0) - 4;
            var16 = var31[var13 + var14 * var34] + var39 / 2;
            var46 = var16 + var42;
            var31[var13 + var14 * var34] = max(var16, var46);

            for(var18 = 0; var18 < var39; ++var18) {
                int32_t var48 = (var18 * this->height + var14) * this->width + var13;
                int32_t var20 = 0;
                if(var18 <= var16) {
                    var20 = Tile::dirt->id;
                }

                if(var18 <= var46) {
                    var20 = Tile::rock->id;
                }

                this->blocks[var48] = (uint8_t)var20;
            }
        }
    }

    minecraft->levelLoadUpdate("Carving..");
    bool var36 = true;
    bool var32 = false;
    var38 = this->width;
    var39 = this->height;
    var29 = this->depth;
    var13 = var38 * var39 * var29 / 256 / 64;

    for (var14 = 0; var14 < var13; ++var14) {
        this->setNextPhase(var14 * 100 / (var13 - 1) / 4);
        float var43 = Util::nextFloat() * (float)var38;
        float var44 = Util::nextFloat() * (float)var29;
        float var47 = Util::nextFloat() * (float)var39;
        var18 = (int32_t)((Util::nextFloat() + Util::nextFloat()) * 75.0f);
        float var49 = (float)((double)Util::nextFloat() * M_PI * 2.0);
        float this0 = 0.0F;
        float this1 = (float)((double)Util::nextFloat() * M_PI * 2.0);
        float var22 = 0.0F;

        for (int32_t var7 = 0; var7 < var18; ++var7) {
            var43 = (float)((double)var43 + sin((double)var49) * cos((double)this1));
            var47 = (float)((double)var47 + cos((double)var49) * cos((double)this1));
            var44 = (float)((double)var44 + sin((double)this1));
            var49 += this0 * 0.2F;
            this0 *= 0.9F;
            this0 += Util::nextFloat() - Util::nextFloat();
            this1 += var22 * 0.5F;
            this1 *= 0.5F;
            var22 *= 0.9F;
            var22 += Util::nextFloat() - Util::nextFloat();
            float var33 = (float)(sin((double)var7 * M_PI / (double)var18) * 2.5 + 1.0);

            for (var34 = (int32_t)(var43 - var33); var34 <= (int32_t)(var43 + var33); ++var34) {
                for (int32_t var12 = (int32_t)(var44 - var33); var12 <= (int32_t)(var44 + var33); ++var12) {
                    for (int32_t var23 = (int32_t)(var47 - var33); var23 <= (int32_t)(var47 + var33); ++var23) {
                        float var24 = (float)var34 - var43;
                        float var25 = (float)var12 - var44;
                        float var26 = (float)var23 - var47;
                        var24 = var24 * var24 + var25 * var25 * 2.0F + var26 * var26;
                        if (var24 < var33 * var33 && var34 >= 1 && var12 >= 1 && var23 >= 1 && var34 < this->width - 1 && var12 < this->depth - 1 && var23 < this->height - 1) {
                            int32_t this2 = (var12 * this->height + var23) * this->width + var34;
                            if(this->blocks[this2] == Tile::rock->id) {
                                this->blocks[this2] = 0;
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
    int64_t var35 = Timer::nanoTime();
    int64_t var40 = 0;
    var13 = Tile::calmWater->id;
    this->setNextPhase(0);

    for (var14 = 0; var14 < this->width; ++var14) {
        var40 += this->floodFillLiquid(var14, this->depth / 2 - 1, 0, 0, var13);
        var40 += this->floodFillLiquid(var14, this->depth / 2 - 1, this->height - 1, 0, var13);
    }

    for (var14 = 0; var14 < this->height; ++var14) {
        var40 += this->floodFillLiquid(0, this->depth / 2 - 1, var14, 0, var13);
        var40 += this->floodFillLiquid(this->width - 1, this->depth / 2 - 1, var14, 0, var13);
    }

    var14 = this->width * this->height / 200;

    for (var42 = 0; var42 < var14; ++var42) {
        if(var42 % 100 == 0) {
            this->setNextPhase(var42 * 100 / (var14 - 1));
        }

        var16 = Util::nextInt(this->width);
        var46 = this->depth / 2 - 1 - Util::nextInt(3);
        var18 = Util::nextInt(this->height);
        if (this->blocks[(var46 * this->height + var18) * this->width + var16] == 0) {
            var40 += this->floodFillLiquid(var16, var46, var18, 0, var13);
        }
    }

    this->setNextPhase(100);
    int64_t var45 = Timer::nanoTime();
    cout << "Flood filled " + to_string(var40) + " tiles in " + to_string((double)(var45 - var35) / 1000000.0) + " ms" << endl;
    minecraft->levelLoadUpdate("Melting..");
    this->addLava();
    minecraft->levelLoadUpdate("Growing..");
    this->addBeaches(var11);
    minecraft->levelLoadUpdate("Planting..");
    this->plantTrees(var11);
    shared_ptr<Level> var28 = make_shared<Level>();
    var28->setData(width, 64, height, this->blocks);
    var28->createTime = Timer::nanoTime() / 1000000;
    var28->creator = name;
    var28->name = "A Nice World";
    return var28;
}

void LevelGen::addBeaches(vector<int32_t>& var1) {
    int32_t width = this->width;
    int32_t height = this->height;
    int32_t depth = this->depth;
    shared_ptr<PerlinNoise> var5 = make_shared<PerlinNoise>(8);
    shared_ptr<PerlinNoise> var6 = make_shared<PerlinNoise>(8);

    for (int32_t var7 = 0; var7 < width; ++var7) {
        setNextPhase(var7 * 100 / (this->width - 1));

        for (int32_t var8 = 0; var8 < height; ++var8) {
            bool var9 = var5->getValue((double)var7, (double)var8) > 8.0;
            bool var10 = var6->getValue((double)var7, (double)var8) > 12.0;
            int32_t var11 = var1[var7 + var8 * width];
            int32_t var12 = (var11 * height + var8) * this->width + var7;
            int32_t var13 = blocks[((var11 + 1) * height + var8) * this->width + var7] & 255;
            
            if ((var13 == Tile::water->id || var13 == Tile::calmWater->id) && var11 <= depth / 2 - 1 && var10) {
                blocks[var12] = (uint8_t)Tile::gravel->id;
            }
            
            if (var13 == 0) {
                int var14 = Tile::grass->id;
                if (var11 <= depth / 2 - 1 && var9) {
                    var14 = Tile::sand->id;
                }

                blocks[var12] = (uint8_t)var14;
            }
        }
    }

}

void LevelGen::plantTrees(vector<int32_t>& var1) {
    int32_t var2 = this->width;
    int32_t var3 = this->width * this->height / 4000;

    for (int32_t var4 = 0; var4 < var3; ++var4) {
        setNextPhase(var4 * 100 / (var3 - 1));
        int32_t var5 = Util::nextInt(this->width);
        int32_t var6 = Util::nextInt(this->height);

        for (int32_t var7 = 0; var7 < 20; ++var7) {
            int32_t var8 = var5;
            int32_t var9 = var6;

            for (int32_t var10 = 0; var10 < 20; ++var10) {
                var8 += Util::nextInt(6) - Util::nextInt(6);
                var9 += Util::nextInt(6) - Util::nextInt(6);
                if (var8 >= 0 && var9 >= 0 && var8 < this->width && var9 < this->height) {
                    int32_t var11 = var1[var8 + var9 * var2] + 1;
                    int32_t var12 = Util::nextInt(3) + 4;
                    bool var13 = true;

                    int32_t var14;
                    int32_t var16;
                    int32_t var17;
                    int32_t var18;
                    for (var14 = var11; var14 <= var11 + 1 + var12; ++var14) {
                        uint8_t var15 = 1;
                        if (var14 >= var11 + 1 + var12 - 2) {
                            var15 = 2;
                        }

                        for (var16 = var8 - var15; var16 <= var8 + var15 && var13; ++var16) {
                            for (var17 = var9 - var15; var17 <= var9 + var15 && var13; ++var17) {
                                if (var16 >= 0 && var14 >= 0 && var17 >= 0 && var16 < this->width && var14 < this->depth && var17 < this->height) {
                                    var18 = blocks[(var14 * this->height + var17) * this->width + var16] & 255;
                                    if (var18 != 0) {
                                        var13 = false;
                                    }
                                }
                                else {
                                    var13 = false;
                                }
                            }
                        }
                    }

                    if (var13) {
                        var14 = (var11 * this->height + var9) * this->width + var8;
                        int32_t var22 = this->blocks[((var11 - 1) * this->height + var9) * this->width + var8] & 255;
                        if (var22 == Tile::grass->id && var11 < this->depth - var12 - 1) {
                            blocks[var14 - 1 * this->width * this->height] = (uint8_t)Tile::dirt->id;

                            for (var16 = var11 - 3 + var12; var16 <= var11 + var12; ++var16) {
                                var17 = var16 - (var11 + var12);
                                var18 = 1 - var17 / 2;

                                for (int32_t var21 = var8 - var18; var21 <= var8 + var18; ++var21) {
                                    var22 = var21 - var8;

                                    for(int32_t var19 = var9 - var18; var19 <= var9 + var18; ++var19) {
                                        int32_t var20 = var19 - var9;
                                        if(abs(var22) != var18 || abs(var20) != var18 || Util::nextInt(2) != 0 && var17 != 0) {
                                            blocks[(var16 * height + var19) * width + var21] = (uint8_t)Tile::leaf->id;
                                        }
                                    }
                                }
                            }

                            for (var16 = 0; var16 < var12; ++var16) {
                                this->blocks[var14 + var16 * this->width * this->height] = (uint8_t)Tile::log->id;
                            }
                        }
                    }
                }
            }
        }
    }

}

void LevelGen::carveTunnels(int32_t var1, int32_t var2, int32_t var3, int32_t var4) {
    uint8_t var25 = (uint8_t)var1;
    var4 = this->width;
    int32_t var5 = this->height;
    int32_t var6 = this->depth;
    int32_t var7 = var4 * var5 * var6 / 256 / 64 * var2 / 100;

    for (int32_t var8 = 0; var8 < var7; var8++) {
        this->setNextPhase(var8 * 100 / (var7 - 1) / 4 + var3 * 100 / 4);
        float var9 = Util::nextFloat() * (float)var4;
        float var10 = Util::nextFloat() * (float)var6;
        float var11 = Util::nextFloat() * (float)var5;
        int var12 = (int32_t)((Util::nextFloat() + Util::nextFloat()) * 75.0F * (float)var2 / 100.0F);
        float var13 = (float)((double)Util::nextFloat() * M_PI * 2.0);
        float var14 = 0.0F;
        float var15 = (float)((double)Util::nextFloat() * M_PI * 2.0);
        float var16 = 0.0F;

        for (int32_t var17 = 0; var17 < var12; var17++) {
            var9 = (float)((double)var9 + sin((double)var13) * cos((double)var15));
            var11 = (float)((double)var11 + cos((double)var13) * cos((double)var15));
            var10 = (float)((double)var10 + sin((double)var15));
            var13 += var14 * 0.2F;
            var14 *= 0.9F;
            var14 += Util::nextFloat() - Util::nextFloat();
            var15 += var16 * 0.5F;
            var15 *= 0.5F;
            var16 *= 0.9F;
            var16 += Util::nextFloat() - Util::nextFloat();
            float var18 = (float)(sin((double)var17 * M_PI / (double)var12) * (double)var2 / 100.0 + 1.0);

            for (int32_t var19 = (int32_t)(var9 - var18); var19 <= (int32_t)(var9 + var18); var19++) {
                for (int32_t var20 = (int32_t)(var10 - var18); var20 <= (int32_t)(var10 + var18); var20++) {
                    for (int32_t var21 = (int32_t)(var11 - var18); var21 <= (int32_t)(var11 + var18); var21++) {
                        float var22 = (float)var19 - var9;
                        float var23 = (float)var20 - var10;
                        float var24 = (float)var21 - var11;
                        var22 = var22 * var22 + var23 * var23 * 2.0F + var24 * var24;
                        if (var22 < var18 * var18 && var19 >= 1 && var20 >= 1 && var21 >= 1 && var19 < this->width - 1 && var20 < this->depth - 1 && var21 < this->height - 1) {
                            int32_t var26 = (var20 * this->height + var21) * this->width + var19;
                            if (this->blocks[var26] == Tile::rock->id) {
                                this->blocks[var26] = var25;
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
    int32_t var1 = 0;
    int32_t var2 = this->width * this->height * this->depth / 10000;

    for (int32_t var3 = 0; var3 < var2; ++var3) {
        if (var3 % 100 == 0) {
            this->setNextPhase(var3 * 100 / (var2 - 1));
        }

        int32_t var4 = Util::nextInt(this->width);
        int32_t var5 = Util::nextInt(this->depth / 2 - 4);
        int32_t var6 = Util::nextInt(this->height);
        if (this->blocks[(var5 * this->height + var6) * this->width + var4] == 0) {
            var1++;
            this->floodFillLiquid(var4, var5, var6, 0, Tile::calmLava->id);
        }
    }

    this->setNextPhase(100);
    cout << "LavaCount: " + to_string(var1) << endl;
}

int64_t LevelGen::floodFillLiquid(int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5) {
    uint8_t var20 = (uint8_t)var5;
    vector<vector<int32_t>> var21 = vector<vector<int32_t>>();
    uint8_t var6 = 0;
    int32_t var7 = 1;

    int32_t var8;
    for (var8 = 1; 1 << var7 < this->width; ++var7) {
    }

    while (1 << var8 < this->height) {
        var8++;
    }

    int32_t var9 = this->height - 1;
    int32_t var10 = this->width - 1;
    int32_t var22 = var6 + 1;
    this->coords[0] = ((var2 << var8) + var3 << var7) + var1;
    int64_t var13 = 0L;
    var1 = this->width * this->height;

    while (var22 > 0) {
        var22--;
        var2 = this->coords[var22];
        if (var22 == 0 && var21.size() > 0) {
            cout << "IT HAPPENED!" << endl;
            this->coords = move(var21.back());
            var21.pop_back();
            var22 = this->coords.size();
        }

        var3 = var2 >> var7 & var9;
        int32_t var11 = var2 >> var7 + var8;
        int32_t var12 = var2 & var10;

        int32_t var15;
        for (var15 = var12; var12 > 0 && this->blocks[var2 - 1] == 0; var2--) {
            var12--;
        }

        while (var15 < this->width && this->blocks[var2 + var15 - var12] == 0) {
            var15++;
        }

        int32_t var16 = var2 >> var7 & var9;
        int32_t var17 = var2 >> var7 + var8;
        if(var16 != var3 || var17 != var11) {
            cout << "hoooly fuck";
        }

        bool var23 = false;
        bool var24 = false;
        bool var18 = false;
        var13 += (int64_t)(var15 - var12);

        for (var12 = var12; var12 < var15; ++var12) {
            this->blocks[var2] = var20;
            bool var19;
            if (var3 > 0) {
                var19 = this->blocks[var2 - this->width] == 0;
                if (var19 && !var23) {
                    if (var22 == this->coords.size()) {
                        var21.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        var22 = 0;
                    }

                    this->coords[var22++] = var2 - this->width;
                }

                var23 = var19;
            }

            if (var3 < this->height - 1) {
                var19 = this->blocks[var2 + this->width] == 0;
                if (var19 && !var24) {
                    if (var22 == this->coords.size()) {
                        var21.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        var22 = 0;
                    }

                    this->coords[var22++] = var2 + this->width;
                }

                var24 = var19;
            }

            if (var11 > 0) {
                uint8_t var25 = this->blocks[var2 - var1];
                if ((var20 == Tile::lava->id || var20 == Tile::calmLava->id) && (var25 == Tile::water->id || var25 == Tile::calmWater->id)) {
                    this->blocks[var2 - var1] = (uint8_t)Tile::rock->id;
                }

                var19 = var25 == 0;
                if (var19 && !var18) {
                    if (var22 == this->coords.size()) {
                        var21.push_back(this->coords);
                        this->coords = vector<int32_t>(1048576);
                        var22 = 0;
                    }

                    this->coords[var22++] = var2 - var1;
                }

                var18 = var19;
            }

            ++var2;
        }
    }

    return var13;
}