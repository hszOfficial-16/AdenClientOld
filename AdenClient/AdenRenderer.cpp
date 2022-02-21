#include "AdenRenderer.h"
using namespace std;

void AdenRenderer::Draw()
{
	if (entityFollowing != nullptr)
		cameraPosition = { entityFollowing->GetPosition().x, entityFollowing->GetPosition().y - 80 };

	SDL_RenderClear(AdenWindow::GetInstance().renderer);
	if (canDrawBackground) DrawBackground();
	if (canDrawBlocks) DrawBlocks();
	if (canDrawGUI) DrawGUI();
}

void AdenRenderer::DrawBackground()
{
	// 背景绘制
	SDL_Rect textureRect = { 0, 0, 1280, 960 };
	SDL_Rect copyRect = { 0, 0,
		AdenWindow::GetInstance().windowWidth,
		AdenWindow::GetInstance().windowHeight
	};
}

void AdenRenderer::DrawBlocks()
{
	unsigned short nowBlockOrder = unsigned int(cameraPosition.x / 960);
	// 地图绘制
	if (nowBlockOrder < nLastBlockOrder) {
		AdenChunk* copyBlock = new AdenChunk();
		*copyBlock = *AdenWorldManager::GetInstance().loadedChunks[2];
		mapIO->SaveBlock(copyBlock, nLastBlockOrder + 1);

		*AdenWorldManager::GetInstance().loadedChunks[2] = *AdenWorldManager::GetInstance().loadedChunks[1];
		*AdenWorldManager::GetInstance().loadedChunks[1] = *AdenWorldManager::GetInstance().loadedChunks[0];
		mapIO->LoadBlock(AdenWorldManager::GetInstance().loadedChunks[0], nowBlockOrder - 1);
		nLastBlockOrder = nowBlockOrder;
	}
	if (nowBlockOrder > nLastBlockOrder) {
		// 将副本交给 IO 线程
		AdenChunk* copyBlock = new AdenChunk();
		*copyBlock = *AdenWorldManager::GetInstance().loadedChunks[0];
		mapIO->SaveBlock(copyBlock, nLastBlockOrder - 1);

		*AdenWorldManager::GetInstance().loadedChunks[0] = *AdenWorldManager::GetInstance().loadedChunks[1];
		*AdenWorldManager::GetInstance().loadedChunks[1] = *AdenWorldManager::GetInstance().loadedChunks[2];
		mapIO->LoadBlock(AdenWorldManager::GetInstance().loadedChunks[2], nowBlockOrder + 1);
		nLastBlockOrder = nowBlockOrder;
	}

	// 计算光照
	//CalculateLightIntensity();

	// 设置好单个方块所需的信息
	SDL_Rect textureRect = { 0, 0, 40, 40 };
	SDL_Rect copyRect = { 0, 0, 30, 30 };
	// 从左到右，方块在三个连起来的区块中的 x 位置
	int xLeft = (cameraPosition.x - AdenWindow::GetInstance().windowWidth / 2) / 30 - 32 * (nLastBlockOrder - 1);
	int xRight = (cameraPosition.x + AdenWindow::GetInstance().windowWidth / 2) / 30 - 32 * (nLastBlockOrder - 1) + 1;
	// 从上到下，方块在区块中的 y 位置
	int yUp = (cameraPosition.y - AdenWindow::GetInstance().windowHeight / 2) / 30;
	int yDown = (cameraPosition.y + AdenWindow::GetInstance().windowHeight / 2) / 30 + 1;

	// 渲染摄像头内所有的背景方块
	for (unsigned short i = xLeft; i < xRight; i++)
		if (AdenWorldManager::GetInstance().chunkType[i / 32 + nLastBlockOrder - 1] != AdenWorldManager::UNLOADED)
			for (unsigned short j = yUp; j < yDown; j++) {
				copyRect.x = (i + (nLastBlockOrder - 1) * 32) * 30 - (cameraPosition.x - AdenWindow::GetInstance().windowWidth / 2);
				copyRect.y = j * 30 - (cameraPosition.y - AdenWindow::GetInstance().windowHeight / 2);

				Uint8 theLightIntensity = lightIntensity[i][j - unsigned short(cameraPosition.y / 30) + 32];

				if (theLightIntensity != 255 && AdenWorldManager::GetInstance().loadedChunks[i / 32]->backBlocks[i % 32][j] != 0)
					SDL_RenderCopy(
						AdenWindow::GetInstance().renderer,
						AdenBlock::GetInstance().pTexture[AdenWorldManager::GetInstance().loadedChunks[i / 32]->backBlocks[i % 32][j]],
						&textureRect,
						&copyRect
					);
			}

	// 渲染摄像头内所有的实体
	if (canDrawEntity) DrawEntity();

	// 渲染摄像头内所有的前景方块
	for (unsigned short i = xLeft; i < xRight; i++)
		if (AdenWorldManager::GetInstance().chunkType[i / 32 + nLastBlockOrder - 1] != AdenWorldManager::UNLOADED)
			for (unsigned short j = yUp; j < yDown; j++) {
				copyRect.x = (i + (nLastBlockOrder - 1) * 32) * 30 - (cameraPosition.x - AdenWindow::GetInstance().windowWidth / 2);
				copyRect.y = j * 30 - (cameraPosition.y - AdenWindow::GetInstance().windowHeight / 2);

				Uint8 theLightIntensity = lightIntensity[i][j - unsigned short(cameraPosition.y / 30) + 32];

				if (theLightIntensity != 255 && AdenWorldManager::GetInstance().loadedChunks[i / 32]->foreBlocks[i % 32][j] != 0)
					SDL_RenderCopy(
						AdenWindow::GetInstance().renderer,
						AdenBlock::GetInstance().pTexture[AdenWorldManager::GetInstance().loadedChunks[i / 32]->foreBlocks[i % 32][j]],
						&textureRect,
						&copyRect
					);

				if (AdenWorldManager::GetInstance().loadedChunks[i / 32]->foreBlocks[i % 32][j] != 0) {
					copyRect.x -= 1; copyRect.y -= 1;
					copyRect.w = 31; copyRect.h = 31;
					SDL_SetRenderDrawColor(AdenWindow::GetInstance().renderer, globalLight.r, globalLight.g, globalLight.b, theLightIntensity);
					SDL_RenderFillRect(AdenWindow::GetInstance().renderer, &copyRect);
				}
			}
}

void AdenRenderer::DrawEntity()
{
	for (AdenEntity* refEntity : AdenEntity::GetManager()) {
		if (refEntity->GetPosition().x > cameraPosition.x - AdenWindow::GetInstance().windowWidth / 2 &&
			refEntity->GetPosition().x < cameraPosition.x + AdenWindow::GetInstance().windowWidth / 2 &&
			refEntity->GetPosition().y > cameraPosition.y - AdenWindow::GetInstance().windowHeight / 2 &&
			refEntity->GetPosition().y < cameraPosition.y + AdenWindow::GetInstance().windowHeight / 2)
			refEntity->Draw(cameraPosition.x, cameraPosition.y);
	}
}

void AdenRenderer::DrawGUI()
{

}

void AdenRenderer::CalculateLightIntensity()
{
	for (unsigned short i = 0; i < 96; i++)
	{
		Uint8 alpha = 0;
		unsigned short top = cameraPosition.y / 30 - 32;
		for (unsigned short j = 0; j < 64; j++)
		{
			if (AdenWorldManager::GetInstance().loadedChunks[i / 32]->foreBlocks[i % 32][j + top] != 0)
			{
				lightIntensity[i][j] = alpha;
				if (alpha > 204) alpha = 255;
				else alpha += 51;
			}
			else if (AdenWorldManager::GetInstance().loadedChunks[i / 32]->foreBlocks[i % 32][j + top] == 0)
			{
				if (alpha < 17) alpha = 0;
				else alpha -= 17;
				lightIntensity[i][j] = alpha;
			}
		}
	}
}

void AdenRenderer::InitCamera(string name, float x, float y)
{
	canDrawBackground = true;
	canDrawBlocks = true;
	canDrawEntity = true;

	cameraPosition = { x, y };
	strWorldName = name;
	nLastBlockOrder = unsigned int(x / 960);

	mapIO = new AdenAsyncMapIO(name);
	for (int i = 0; i < 3; i++)
	{
		AdenWorldManager::GetInstance().loadedChunks[i] = new AdenChunk();
		mapIO->LoadBlock(AdenWorldManager::GetInstance().loadedChunks[i], nLastBlockOrder - 1 + i);
	}
}

void AdenRenderer::DestroyCamera()
{
	canDrawBackground = false;
	canDrawBlocks = false;
	canDrawEntity = false;
	for (int i = 0; i < 3; i++)
	{
		delete AdenWorldManager::GetInstance().loadedChunks[i];
		AdenWorldManager::GetInstance().loadedChunks[i] = nullptr;
	}
	delete mapIO;
}