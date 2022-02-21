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
#include "AdenGUI.h"							// ��Ⱦ UI ���
#include "AdenBlock.h"							// ��ȡ������Ϣ
#include "AdenWorld.h"							// ��Ⱦ��ͼ
#include "AdenEntity.h"							// ��Ⱦʵ��

struct AdenColor
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

class AdenRenderer
{
public:
	// ����һ��֡�Ļ���
	void Draw();

	// �����
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

	void CalculateLightIntensity();				// �������

private:
	bool canDrawGUI = true;
	bool canDrawBackground = false;
	bool canDrawBlocks = false;
	bool canDrawEntity = false;

	// ��Ϸ����ͷ״̬
	SDL_FPoint cameraPosition = { 0, 0 };		// λ�ڵ�����
	AdenEntity* entityFollowing = nullptr;		// �����ʵ��

	AdenColor globalLight = { 0, 0, 0 };		// ȫ�ֹ�����ɫ
	Uint8 lightIntensity[96][64];				// �������ǿ��

	std::string strWorldName;					// ��Ⱦ������
	unsigned short nLastBlockOrder = 0;			// ��֡��������
	AdenAsyncMapIO* mapIO = nullptr;			// ��̬��������

	AdenRenderer() {
		for (int i = 0; i < 96; i++)
			for (int j = 0; j < 64; j++)
				lightIntensity[i][j] = 0;
	}
};

#endif