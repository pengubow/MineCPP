#pragma once

#include "level/Level.h"
#include "level/levelgen/synth/PerlinNoise.h"
#include "level/levelgen/synth/Distort.h"

class Minecraft;

class LevelGen : public enable_shared_from_this<LevelGen> {
	weak_ptr<Minecraft> minecraft;
	int32_t width;
	int32_t height;
	int32_t depth;
	vector<uint8_t> blocks;
	vector<int32_t> coords = vector<int32_t>(1048576);
public:
	LevelGen(shared_ptr<Minecraft> minecraft);

	shared_ptr<Level> generateLevel(string name, int32_t width, int32_t height, int32_t depth);
private:
	void addBeaches(vector<int32_t>& var1);
	void plantTrees(vector<int32_t>& var1);
	void carveTunnels(int32_t var1, int32_t var2, int32_t var3, int32_t var4);
	void setNextPhase(int32_t phase);
	void addLava();
	int64_t floodFillLiquid(int32_t var1, int32_t var2, int32_t var3, int32_t var4, int32_t var5);
};