#ifndef _ADEN_INSTANCE_H_
#define _ADEN_INSTANCE_H_

// SDL Support
#include <SDL.h>

// Concurrent Support
#include <mutex>
#include <condition_variable>

class AdenWindow
{
public:
	~AdenWindow() {}
	AdenWindow(const AdenWindow&) = delete;
	AdenWindow& operator=(const AdenWindow&) = delete;
	static AdenWindow& GetInstance()
	{
		static AdenWindow instance;
		return instance;
	}

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	Uint32 frameAmount = 60;
	int windowWidth = 0, windowHeight = 0;

private:
	AdenWindow() {}
};

// 与 SDL_Timer不同的是，AdenTimer 以帧为单位
class AdenTimer
{
public:
	~AdenTimer() {}
	AdenTimer(const AdenTimer&) = delete;
	AdenTimer& operator=(const AdenTimer&) = delete;
	static AdenTimer& GetInstance()
	{
		static AdenTimer instance;
		return instance;
	}

	Uint32 currentFrame = 0;

private:
	AdenTimer() {}
};

class AdenAttribute
{
public:
	~AdenAttribute() {}
	AdenAttribute(const AdenAttribute&) = delete;
	AdenAttribute& operator=(const AdenAttribute&) = delete;
	static AdenAttribute& GetInstance()
	{
		static AdenAttribute instance;
		return instance;
	}
	
	float fGravityValue = 0.5f;

private:
	AdenAttribute() {}
};

#endif