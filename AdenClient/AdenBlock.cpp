#include "AdenBlock.h"

AdenBlock::AdenBlock()
{
	canMovingIn[AIR] = true;
	nHardness[AIR] = 0;
	nIllumination[AIR] = 0;
	pTexture[AIR] = nullptr;

	const char* path = "resource/image/grass_dirt.png";
	SDL_Surface* pSurface = IMG_Load(path);
	canMovingIn[GRASS_DIRT] = false;
	nHardness[GRASS_DIRT] = 40;
	nIllumination[GRASS_DIRT] = 0;
	pTexture[GRASS_DIRT] = SDL_CreateTextureFromSurface(AdenWindow::GetInstance().renderer, pSurface);

	path = "resource/image/dirt.png";
	pSurface = IMG_Load(path);
	canMovingIn[DIRT] = false;
	nHardness[DIRT] = 40;
	nIllumination[DIRT] = 0;
	pTexture[DIRT] = SDL_CreateTextureFromSurface(AdenWindow::GetInstance().renderer, pSurface);

	path = "resource/image/stone.png";
	pSurface = IMG_Load(path);
	canMovingIn[STONE] = false;
	nHardness[STONE] = 40;
	nIllumination[STONE] = 0;
	pTexture[STONE] = SDL_CreateTextureFromSurface(AdenWindow::GetInstance().renderer, pSurface);
}

AdenBlock::~AdenBlock()
{
	for (int i = 1; i <= 3; i++)
		SDL_DestroyTexture(pTexture[i]);
}