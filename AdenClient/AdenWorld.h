#ifndef _ADEN_WORLD_H_
#define _ADEN_WORLD_H_

// File IO Support
#include <fstream>
#include <string>

// Concurrent Support
#include <thread>
#include <chrono>
#include <list>

// Random Number Generator
#include <stdlib.h>
#include <time.h>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"
#include "AdenBlock.h"

// 区块结构
struct AdenChunk
{
	// 区块种类
	enum ChunkType
	{
		NORMAL = 0,					// 普通区块
	};

	ChunkType type;
	Uint16 backBlocks[32][512];		// 背景层方块
	Uint16 foreBlocks[32][512];		// 前景层方块
};

// Aden世界
struct AdenWorld
{
	// 世界大小
	enum WorldSize {
		SMALL = 0,
		MIDDLE,
		BIG
	};

	// 世界难度
	enum WorldDifficulty {
		EASY = 0,
		NORMAL,
		HARD
	};

	WorldSize size;
	WorldDifficulty difficulty;
	unsigned int seed;								// 种子
};

// 异步线程读取地图
class AdenAsyncMapIO
{
public:
	enum RequestType {
		LOADING = 0,
		SAVING
	};

	struct IORequest {
		AdenChunk* chunk;
		unsigned short order;
		RequestType requestType;
	};

	AdenAsyncMapIO(std::string);

	~AdenAsyncMapIO() {
		quitIO = true;
		file.close(); file.clear();
	}
	void LoadBlock(AdenChunk* chunk, unsigned short order) { taskList.push_back({ chunk, order, LOADING }); }
	void SaveBlock(AdenChunk* chunk, unsigned short order) { taskList.push_back({ chunk, order, SAVING }); }

private:
	std::thread threadWorking;						// IO 线程
	std::list<IORequest> taskList;					// IO 请求队列

	bool quitIO = false;
	std::fstream file;
};

class AdenWorldManager
{
public:
	// 区块状态
	enum ChunkState {
		LOADED = 0,										// 加载区块(进行逻辑更新和渲染)
		ACTIVE,											// 活跃区块(仍然进行逻辑更新)
		UNLOADED										// 未加载区块(保存在文件中)
	};

	~AdenWorldManager() {}
	AdenWorldManager(const AdenWorldManager&) = delete;
	AdenWorldManager& operator=(const AdenWorldManager&) = delete;
	static AdenWorldManager& GetInstance()
	{
		static AdenWorldManager instance;
		return instance;
	}

	void GenerateWorld(std::string);

	AdenChunk* loadedChunks[3];
	ChunkState chunkType[256];

private:
	AdenWorldManager() {
		for (int i = 0; i < 3; i++)
			loadedChunks[i] = nullptr;

		for (int i = 0; i < 256; i++)
			chunkType[i] = UNLOADED;
	}
};

class PerlinNoise
{
public:
	PerlinNoise(unsigned int seed, unsigned int capacity):
		nSeed(seed), nLatticeCapacity(capacity) {}
	~PerlinNoise() {}

public:
	float Generate1D(unsigned int);
	float Generate2D(unsigned int, unsigned int);

protected:
	// 伪随机向量选择，并返回点乘结果
	float Grad1D(unsigned int, float);
	float Grad2D(unsigned int, float, float);

	// 缓冲函数
	float Fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	// 晶格顶点之间的点的值
	float Lerp(float weight, float a, float b)
	{
		return a + weight * (b - a);
	}
private:
	// 该柏林算法生成 [-1, 1] 的数字时，会根据坐标值和种子产生哈希值
	// 最后根据这个哈希值在 perm 数组中伪随机地选择向量
	unsigned int nSeed = 1;
	unsigned int nLatticeCapacity;
	static int perm[256];
};

#endif