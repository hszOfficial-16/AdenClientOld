#include "AdenListener.h"
using namespace std;

AdenSDLListener::AdenSDLListener() :
	eventType(SDL_FIRSTEVENT), funcCallback([](const SDL_Event&) {})
{

}

AdenSDLListener::AdenSDLListener(Uint32 type, function<void(const SDL_Event&)> func = [](const SDL_Event&) {}) :
	eventType(type), funcCallback(func)
{
	AdenListenerManager::GetInstance().RegisterListener(this);
}

bool AdenListenerManager::CollectInput()
{
	while (SDL_PollEvent(&theEvent))
	{
		if (theEvent.type == SDL_QUIT)
			return true;
		else for (AdenSDLListener* pListener : vListener)
			if (pListener->GetType() == theEvent.type)
				pListener->GetCallback()(theEvent);
	}
	return false;
}