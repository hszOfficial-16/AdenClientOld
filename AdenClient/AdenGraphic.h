#ifndef _ADEN_GRAPHIC_H_
#define _ADEN_GRAPHIC_H_

// SDL Support
#include <SDL.h>
#include <SDL_image.h>

// STL Support
#include <vector>
#include <string>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"

class AdenImage
{
public:
	AdenImage();
	AdenImage(std::string);
	~AdenImage() { SDL_DestroyTexture(pTexture); }

	void LoadImageWithPath(std::string);
	int GetWidth() { return nWidth; }
	int GetHeight() { return nHeight; }
	SDL_Texture* GetTexture() { return pTexture; }

private:
	int nWidth, nHeight;
	SDL_Texture* pTexture;
};

class AdenAnimation
{
public:
	AdenAnimation() {}
	~AdenAnimation();

	void OneStep();
	void AddFrameWithPath(std::string, Uint32);
	void AddFrameWithImage(AdenImage*, Uint32);

	void SetDefault()
	{
		nPlayProgress = 0;
		nCurrentFrame = 0;
	}

	struct AdenFrame
	{
		Uint32 nDuration;
		AdenImage* pImage;
	};
	AdenFrame GetFrame() { return *vFrames[nCurrentFrame]; }

private:
	Uint32 nPlayProgress = 0;
	size_t nCurrentFrame = 0;
	std::vector<AdenFrame*> vFrames;
};


#endif // !_ADEN_GRAPHIC_H_