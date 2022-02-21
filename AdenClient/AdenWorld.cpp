#include "AdenWorld.h"
using namespace std;

AdenAsyncMapIO::AdenAsyncMapIO(string mapName)
{
	file.open(mapName + ".aw", ios_base::in | ios_base::out | ios_base::binary);

	threadWorking = thread([this]() {
		while (!quitIO) {
			// 倘若队列里没有请求，则睡眠
			while (taskList.empty())
				this_thread::sleep_for(chrono::milliseconds(10));

			IORequest request = taskList.back();
			taskList.pop_back();

			if (request.requestType == LOADING) {
				file.seekg(request.order * sizeof(AdenChunk) + sizeof(AdenWorld));
				file.read((char*)request.chunk, sizeof(AdenChunk));
				AdenWorldManager::GetInstance().chunkType[request.order] = AdenWorldManager::LOADED;
			}
			else if (request.requestType == SAVING) {
				file.seekp(request.order * sizeof(AdenChunk) + sizeof(AdenWorld));
				file.write((char*)request.chunk, sizeof(AdenChunk));
				delete request.chunk;		//删除副本
				AdenWorldManager::GetInstance().chunkType[request.order] = AdenWorldManager::UNLOADED;
			}
		}});
	
	threadWorking.detach();
}

void AdenWorldManager::GenerateWorld(string name)
{
	// 流水线化操作生成世界，并将世界存入一个文件中
	// 开头存一些世界的信息，后面存所有区块的数据
	// 使用柏林噪音算法生成一个地图

	AdenChunk* loadingWorld[256];
	ofstream fout(name + ".aw", ios_base::out | ios_base::binary);

	// 设置一个世界的属性
	AdenWorld world = { AdenWorld::MIDDLE, AdenWorld::NORMAL, 114514 };

	// 生成世界
	unsigned int i, j;
	for (i = 0; i < 256; i++)
		loadingWorld[i] = new AdenChunk();

	PerlinNoise noisePerlin = PerlinNoise(world.seed, 128);

	for (i = 0; i < 8192; i++)
	{
		for (j = 0; j < 512; j++)
		{
			loadingWorld[i / 32]->foreBlocks[i % 32][j] = AdenBlock::AIR;
			loadingWorld[i / 32]->backBlocks[i % 32][j] = AdenBlock::AIR;
		}
		loadingWorld[i / 32]->foreBlocks[i % 32][256 + int(noisePerlin.Generate1D(i) * 8)] = AdenBlock::GRASS_DIRT;
		for (j = 256 + int(noisePerlin.Generate1D(i) * 8) + 1; j < 512; j++)
		{
			loadingWorld[i / 32]->foreBlocks[i % 32][j] = AdenBlock::DIRT;
		}
	}

	// 存入文件
	fout.write((char*)&world, sizeof(AdenWorld));
	for (i = 0; i < 256; i++)
	{
		fout.write((char*)loadingWorld[i], sizeof(AdenChunk));
		delete loadingWorld[i];
	}
	fout.close(); fout.clear();
}

int PerlinNoise::perm[256] = {
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

float PerlinNoise::Grad1D(unsigned int hash, float x)
{
	switch (hash & 1)
	{
	case 0: return x;
	case 1: return -x;
	default: return 0;
	}
}

float PerlinNoise::Grad2D(unsigned int hash, float x, float y)
{
	switch (hash & 3)
	{
	case 0: return x + y;
	case 1: return -x + y;
	case 2: return x - y;
	case 3: return -x - y;
	default: return 0;
	}
}

float PerlinNoise::Generate1D(unsigned int nBlockX)
{
	unsigned int nHashX = (nBlockX * nSeed) & 255;
	float fBlockX = (nBlockX % 32) / 32.0f;

	return Lerp(Fade(fBlockX),
		Grad1D(perm[nHashX], fBlockX),
		Grad1D(perm[(nHashX + 1) & 255], fBlockX - 1));
}

float PerlinNoise::Generate2D(unsigned int nBlockX, unsigned int nBlockY)
{
	unsigned int nHashX = (nBlockX * nSeed) & 255;
	unsigned int nHashY = (nBlockY * nSeed) & 255;
	float fBlockX = (nBlockX % 32) / 32.0f;
	float fBlockY = (nBlockY % 32) / 32.0f;

	unsigned int LeftUp = perm[nHashX] + nHashY;
	unsigned int RightUp = perm[(nHashX + 1) & 255] + nHashY;

	return Lerp(Fade(fBlockY),
		Lerp(Fade(fBlockX), Grad2D(LeftUp, fBlockX, fBlockY), Grad2D(RightUp, fBlockX - 1, fBlockY)),
		Lerp(Fade(fBlockX), Grad2D((LeftUp + 1) & 255, fBlockX, fBlockY - 1), Grad2D((RightUp + 1) & 255, fBlockX - 1, fBlockY - 1)));
}