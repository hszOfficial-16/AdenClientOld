#ifndef _ADEN_RENDERER_H_
#define _ADEN_RENDERER_H_

// SDL Support
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"
#include "AdenGUI.h"							// 渲染 UI 组件
#include "AdenBlock.h"							// 获取方块信息
#include "AdenWorld.h"							// 渲染地图
#include "AdenEntity.h"							// 渲染实体

struct AdenColor
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

class AdenRenderer
{
public:
	// 进行一个帧的绘制
	void Draw();

	// 摄像机
	void InitCamera(std::string, float x, float y);
	void DestroyCamera();

	void SetPosition(float x, float y) { cameraPosition = { x, y }; }
	void FollowEntity(AdenEntity* entity) { entityFollowing = entity; }
	void SetGlobalLightColor(Uint8 r, Uint8 g, Uint8 b) { globalLight = { r, g, b }; }

	~AdenRenderer() {}
	AdenRenderer(const AdenRenderer&) = delete;
	AdenRenderer& operator=(const AdenRenderer&) = delete;
	static AdenRenderer& GetInstance()
	{
		static AdenRenderer instance;
		return instance;
	}

protected:
	void DrawGUI();
	void DrawBackground();
	void DrawBlocks();
	void DrawEntity();

	void CalculateLightIntensity();				// 计算光照

private:
	bool canDrawGUI = true;
	bool canDrawBackground = false;
	bool canDrawBlocks = false;
	bool canDrawEntity = false;

	// 游戏摄像头状态
	SDL_FPoint cameraPosition = { 0, 0 };		// 位于的坐标
	AdenEntity* entityFollowing = nullptr;		// 跟随的实体

	AdenColor globalLight = { 0, 0, 0 };		// 全局光照颜色
	Uint8 lightIntensity[96][64];				// 方块光照强度

	std::string strWorldName;					// 渲染的世界
	unsigned short nLastBlockOrder = 0;			// 上帧所处区块
	AdenAsyncMapIO* mapIO = nullptr;			// 动态加载区块

	AdenRenderer() {
		for (int i = 0; i < 96; i++)
			for (int j = 0; j < 64; j++)
				lightIntensity[i][j] = 0;
	}
};

#endif