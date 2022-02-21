#ifndef _ADEN_LOGIC_H_
#define _ADEN_LOGIC_H_

// SDL Support
#include <SDL.h>

// Concurrent Support
#include <thread>

// Math Support
#include <cmath>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"
#include "AdenRenderer.h"					// ������Ⱦ��
#include "AdenListener.h"					// ���¼�����
#include "AdenBlock.h"						// ��������

class AdenUpdater
{
public:
	void Run();

	~AdenUpdater();
	AdenUpdater(const AdenUpdater&) = delete;
	AdenUpdater& operator=(const AdenUpdater&) = delete;
	static AdenUpdater& GetInstance()
	{
		static AdenUpdater instance;
		return instance;
	}
private:
	bool isQuit = false;
	Uint32 timeGameStart;

	AdenSDLListener* listenWindow;

	AdenUpdater();
};

#endif // !_ADEN_LOGIC_H_