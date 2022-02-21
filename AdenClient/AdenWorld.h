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

// ����ṹ
struct AdenChunk
{
	// ��������
	enum ChunkType
	{
		NORMAL = 0,					// ��ͨ����
	};

	ChunkType type;
	Uint16 backBlocks[32][512];		// �����㷽��
	Uint16 foreBlocks[32][512];		// ǰ���㷽��
};

// Aden����
struct AdenWorld
{
	// �����С
	enum WorldSize {
		SMALL = 0,
		MIDDLE,
		BIG
	};

	// �����Ѷ�
	enum WorldDifficulty {
		EASY = 0,
		NORMAL,
		HARD
	};

	WorldSize size;
	WorldDifficulty difficulty;
	unsigned int seed;								// ����
};

// �첽�̶߳�ȡ��ͼ
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
	std::thread threadWorking;						// IO �߳�
	std::list<IORequest> taskList;					// IO �������

	bool quitIO = false;
	std::fstream file;
};

class AdenWorldManager
{
public:
	// ����״̬
	enum ChunkState {
		LOADED = 0,										// ��������(�����߼����º���Ⱦ)
		ACTIVE,											// ��Ծ����(��Ȼ�����߼�����)
		UNLOADED										// δ��������(�������ļ���)
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
	// α�������ѡ�񣬲����ص�˽��
	float Grad1D(unsigned int, float);
	float Grad2D(unsigned int, float, float);

	// ���庯��
	float Fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	// ���񶥵�֮��ĵ��ֵ
	float Lerp(float weight, float a, float b)
	{
		return a + weight * (b - a);
	}
private:
	// �ð����㷨���� [-1, 1] ������ʱ�����������ֵ�����Ӳ�����ϣֵ
	// �����������ϣֵ�� perm ������α�����ѡ������
	unsigned int nSeed = 1;
	unsigned int nLatticeCapacity;
	static int perm[256];
};

#endif