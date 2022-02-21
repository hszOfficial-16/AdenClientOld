#include "AdenMain.h"
using namespace std;

#undef main // 消除 SDL_main 的影响
int main(int argc, char* argv[])
{
	// 初始化lua虚拟机
	lua_State* pLuaState = luaL_newstate();
	luaL_openlibs(pLuaState);
	lua_gc(pLuaState, LUA_GCINC, 100);

	// SDL初始化
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
	TTF_Init();

	// 读取配置文件
	if (!luaL_dofile(pLuaState, "config.lua"))
	{
		lua_getglobal(pLuaState, "windowWidth");
		AdenWindow::GetInstance().windowWidth = (int)lua_tointeger(pLuaState, -1);
		lua_getglobal(pLuaState, "windowHeight");
		AdenWindow::GetInstance().windowHeight = (int)lua_tointeger(pLuaState, -1);
		lua_getglobal(pLuaState, "frameAmount");
		AdenWindow::GetInstance().frameAmount = (Uint32)lua_tointeger(pLuaState, -1);
	}

	// 初始化窗口
	AdenWindow::GetInstance().window = SDL_CreateWindow(
		"Aden",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		AdenWindow::GetInstance().windowWidth,
		AdenWindow::GetInstance().windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	SDL_SetWindowMaximumSize(AdenWindow::GetInstance().window, 1280, 960);
	SDL_SetWindowMinimumSize(AdenWindow::GetInstance().window, 640, 480);

	AdenWindow::GetInstance().renderer = 
		SDL_CreateRenderer(AdenWindow::GetInstance().window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(AdenWindow::GetInstance().renderer, SDL_BLENDMODE_BLEND);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	// 初始化 Aden 世界
	AdenWorldManager::GetInstance().GenerateWorld("MyFirstWorld");

	// 初始化游戏摄像头，玩家默认生成在地图中心
	AdenRenderer::GetInstance().InitCamera("MyFirstWorld", 10620.0, 7600.0);
	AdenPlayer player = AdenPlayer({ 10720.0, 7539.0 }, AdenPlayer::SELF);
	AdenRenderer::GetInstance().FollowEntity(&player);
	AdenUpdater::GetInstance().Run();

	TTF_Quit(); IMG_Quit();
	Mix_CloseAudio(); Mix_Quit();

	if (AdenWindow::GetInstance().renderer)
	{
		SDL_DestroyRenderer(AdenWindow::GetInstance().renderer);
		AdenWindow::GetInstance().renderer = nullptr;
	}
	if (AdenWindow::GetInstance().window)
	{
		SDL_DestroyWindow(AdenWindow::GetInstance().window);
		AdenWindow::GetInstance().window = nullptr;
	}

	SDL_Quit();
	return 0;
}