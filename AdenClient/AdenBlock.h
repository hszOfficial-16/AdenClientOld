#ifndef _ADEN_BLOCK_H_
#define _ADEN_BLOCK_H_

#define ADEN_TOTAL_BLOCKS 256

// SDL Support
#include <SDL.h>
#include <SDL_image.h>

// ModuleInclude
#include "AdenMacro.h"
#include "AdenInstance.h"

class AdenBlock
{
public:
	enum BlockType
	{
		AIR = 0,
		GRASS_DIRT,
		DIRT,
		STONE,
	};

	~AdenBlock();
	AdenBlock(const AdenBlock&) = delete;
	AdenBlock& operator=(const AdenBlock&) = delete;
	static AdenBlock& GetInstance()
	{
		static AdenBlock instance;
		return instance;
	}

	bool canMovingIn[ADEN_TOTAL_BLOCKS];
	short nHardness[ADEN_TOTAL_BLOCKS];
	Uint8 nIllumination[ADEN_TOTAL_BLOCKS];
	SDL_Texture* pTexture[ADEN_TOTAL_BLOCKS];

private:
	AdenBlock();
};

#endif // !_ADEN_BLOCK_H_