#ifndef _ADEN_GUI_H_
#define _ADEN_GUI_H_

// SDL Support
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// STL Support
#include <vector>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"

// 基础的 GUI 组件
class AdenGUI
{
public:
	AdenGUI() {}
	~AdenGUI() {}

	static std::vector<AdenGUI*>& GetInstance()
	{
		static std::vector<AdenGUI*> instance;
		return instance;
	}
private:

};

#endif // !_ADEN_GUI_H_