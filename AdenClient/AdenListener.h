#ifndef _ADEN_GAME_EVENT_H_
#define _ADEN_GAME_EVENT_H_

// SDL Support
#include <SDL.h>

// Concurrent Support
#include <thread>
#include <chrono>

// STL Support
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"

class AdenSDLListener
{
public:
	AdenSDLListener();
	AdenSDLListener(Uint32, std::function<void(const SDL_Event&)>);

	Uint32 GetType() { return eventType; }
	void SetType(Uint32 type) { eventType = type; }

	std::function<void(const SDL_Event&)> GetCallback() { return funcCallback; }
	void SetCallback(std::function<void(const SDL_Event&)> func) { funcCallback = func; }

	void Destroy() { shouldRelease = true; }
	bool ShouldRelease() { return shouldRelease; }

private:
	Uint32 eventType;
	std::function<void(const SDL_Event&)> funcCallback;
	bool shouldRelease = false;
};

class AdenListenerManager
{
public:
	static AdenListenerManager& GetInstance()
	{
		static AdenListenerManager instance;
		return instance;
	}
	bool CollectInput();			// 采集输入
	void RegisterListener(AdenSDLListener* listener) { vListener.push_back(listener); }

	// 这个奇怪的 API 用于 Logic 管理所有监听者的释放
	std::vector<AdenSDLListener*>& GetManager() { return vListener; }

	~AdenListenerManager() {}
	AdenListenerManager(const AdenListenerManager&) = delete;
	AdenListenerManager& operator=(const AdenListenerManager&) = delete;

private:
	SDL_Event theEvent;
	std::vector<AdenSDLListener*> vListener;

	AdenListenerManager() {}
};

#endif // !_ADEN_GAME_EVENT_H_