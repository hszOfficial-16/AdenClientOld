#include "AdenGraphic.h"
using namespace std;

AdenImage::AdenImage() :
	nWidth(0), nHeight(0), pTexture(nullptr)
{

}

AdenImage::AdenImage(string path)
{
	SDL_Surface* campSurface = IMG_Load(path.c_str());
	nWidth = campSurface->w;
	nHeight = campSurface->h;
	pTexture = SDL_CreateTextureFromSurface(AdenWindow::GetInstance().renderer, campSurface);
	delete campSurface;
}

void AdenImage::LoadImageWithPath(string path)
{
	SDL_Surface* campSurface = IMG_Load(path.c_str());
	nWidth = campSurface->w;
	nHeight = campSurface->h;
	pTexture = SDL_CreateTextureFromSurface(AdenWindow::GetInstance().renderer, campSurface);
	delete campSurface;
}

AdenAnimation::~AdenAnimation()
{
	for (AdenFrame*& pFrame : vFrames)
	{
		delete pFrame->pImage;
		delete pFrame;
	}
}

void AdenAnimation::OneStep()
{
	if (nPlayProgress < vFrames[nCurrentFrame]->nDuration)
		nPlayProgress++;
	else
	{
		nPlayProgress = 0;
		nCurrentFrame = (nCurrentFrame + 1) % vFrames.size();
	}
}

void AdenAnimation::AddFrameWithPath(string path, Uint32 duration)
{
	AdenFrame* pFrame = new AdenFrame();
	pFrame->pImage = new AdenImage(path);
	pFrame->nDuration = duration;
	vFrames.push_back(pFrame);
}

void AdenAnimation::AddFrameWithImage(AdenImage* image, Uint32 duration)
{
	AdenFrame* pFrame = new AdenFrame();
	AdenImage* pImage = new AdenImage(*image);
	pFrame->pImage = pImage;
	pFrame->nDuration = duration;
	vFrames.push_back(pFrame);
}