#include "AdenUpdater.h"

#include<iostream>
using namespace std;

AdenUpdater::AdenUpdater()
{
	// ��ʼ����Ϸ��ʼʱ��ʱ��
	timeGameStart = SDL_GetTicks();
	
	// ע�ᴰ�ڼ���������Ҫ�Ǽ������ڴ�С�ı�
	listenWindow = new AdenSDLListener(SDL_WINDOWEVENT, [this](const SDL_Event& theEvent) {
		switch (theEvent.window.event)
		{
		case SDL_WINDOWEVENT_RESIZED:
			SDL_GetWindowSize(
				AdenWindow::GetInstance().window,
				&AdenWindow::GetInstance().windowWidth,
				&AdenWindow::GetInstance().windowHeight
			);
			break;
		default:
			break;
		}
		});
}

AdenUpdater::~AdenUpdater()
{
	delete listenWindow;
	AdenRenderer::GetInstance().DestroyCamera();
}

void AdenUpdater::Run()
{
	while (!isQuit)
	{
		// ���ڴ����
		vector<AdenEntity*> vEntity = AdenEntity::GetManager();
		for (auto iter = vEntity.begin(); iter != vEntity.end(); iter++)
			if ((*iter)->ShouldRelease()) {
				delete (*iter);
				vEntity.erase(iter);
			}

		vector<AdenSDLListener*> vListener = AdenListenerManager::GetInstance().GetManager();
		for (auto iter = vListener.begin(); iter != vListener.end(); iter++)
			if ((*iter)->ShouldRelease()) {
				delete (*iter);
				vListener.erase(iter);
			}

		vector<AdenDecisionArea*> vAttackDecision = AdenDecisionArea::GetManager(AdenDecisionArea::ATTACKDECISION);
		for (auto iter = vAttackDecision.begin(); iter != vAttackDecision.end(); iter++)
			if ((*iter)->ShouldRelease()) {
				delete (*iter);
				vAttackDecision.erase(iter);
			}

		vector<AdenDecisionArea*> vBeatenDecision = AdenDecisionArea::GetManager(AdenDecisionArea::BEATENDECISION);
		for (auto iter = vBeatenDecision.begin(); iter != vBeatenDecision.end(); iter++)
			if ((*iter)->ShouldRelease()) {
				delete (*iter);
				vBeatenDecision.erase(iter);
			}

		vector<AdenDecisionArea*> vCollisionArea = AdenDecisionArea::GetManager(AdenDecisionArea::COLLISIONBOX);
		for (auto iter = vCollisionArea.begin(); iter != vCollisionArea.end(); iter++)
			if ((*iter)->ShouldRelease()) {
				delete (*iter);
				vCollisionArea.erase(iter);
			}

		// ������ɼ� && �¼�����
		isQuit = AdenListenerManager::GetInstance().CollectInput();

		// ���߼�����

		// ��������ʵ����ٶ�
		for (auto iter = vEntity.begin(); iter != vEntity.end(); iter++)
		{
			if ((*iter)->GetHorizontalState() == AdenEntity::LEFT)
			{
				if ((*iter)->GetSpeed().x - (*iter)->GetAcceleration().x > -(*iter)->GetMaxSpeed().x)
					(*iter)->SetSpeed({ (*iter)->GetSpeed().x - (*iter)->GetAcceleration().x, (*iter)->GetSpeed().y });
				else (*iter)->SetSpeed({ -(*iter)->GetMaxSpeed().x, (*iter)->GetSpeed().y });
			}

			else if ((*iter)->GetHorizontalState() == AdenEntity::RIGHT)
			{
				if ((*iter)->GetSpeed().x + (*iter)->GetAcceleration().x < (*iter)->GetMaxSpeed().x)
					(*iter)->SetSpeed({ (*iter)->GetSpeed().x + (*iter)->GetAcceleration().x, (*iter)->GetSpeed().y });
				else (*iter)->SetSpeed({ (*iter)->GetMaxSpeed().x, (*iter)->GetSpeed().y });
			}
			else
			{
				if ((*iter)->GetSpeed().x < 0.0f)
				{
					if ((*iter)->GetSpeed().x + (*iter)->GetAcceleration().x < 0.0f)
						(*iter)->SetSpeed({ (*iter)->GetSpeed().x + (*iter)->GetAcceleration().x, (*iter)->GetSpeed().y });
					else (*iter)->SetSpeed({ 0.0f, (*iter)->GetSpeed().y });
				}
				else if ((*iter)->GetSpeed().x > 0.0f)
				{
					if ((*iter)->GetSpeed().x - (*iter)->GetAcceleration().x > 0.0f)
						(*iter)->SetSpeed({ (*iter)->GetSpeed().x - (*iter)->GetAcceleration().x, (*iter)->GetSpeed().y });
					else (*iter)->SetSpeed({ 0.0f, (*iter)->GetSpeed().y });
				}
			}

			if ((*iter)->GetVerticalState() == AdenEntity::UP)
			{
				if ((*iter)->GetSpeed().y - (*iter)->GetAcceleration().y > -(*iter)->GetMaxSpeed().y)
					(*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetSpeed().y - (*iter)->GetAcceleration().y });
				else (*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetMaxSpeed().y });
			}
			else if ((*iter)->GetVerticalState() == AdenEntity::DOWN)
			{
				if ((*iter)->GetSpeed().y + (*iter)->GetAcceleration().y < (*iter)->GetMaxSpeed().y)
					(*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetSpeed().y + (*iter)->GetAcceleration().y });
				else (*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetMaxSpeed().y });
			}
			else
			{
				if ((*iter)->GetSpeed().y < 0.0f)
				{
					if ((*iter)->GetSpeed().y + (*iter)->GetAcceleration().y < 0.0f)
						(*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetSpeed().y + (*iter)->GetAcceleration().y });
					else (*iter)->SetSpeed({ (*iter)->GetSpeed().x, 0.0f });
				}
				else if ((*iter)->GetSpeed().y > 0.0f)
				{
					if ((*iter)->GetSpeed().y - (*iter)->GetAcceleration().y > 0.0f)
						(*iter)->SetSpeed({ (*iter)->GetSpeed().x, (*iter)->GetSpeed().y - (*iter)->GetAcceleration().y });
					else (*iter)->SetSpeed({ (*iter)->GetSpeed().x, 0.0f });
				}
			}
		}

		// ������ײ�����ײ�߼�������ʵ���λ��
		for (auto iter = vCollisionArea.begin(); iter != vCollisionArea.end(); iter++)
		{
			// ����ˮƽ�ϵ�λ��
			bool isHorizonHit = false;
			unsigned int blockUpY = ((*iter)->GetEntity()->GetPosition().y + (*iter)->GetFRect().y) / 30;
			unsigned int blockDownY = ((*iter)->GetEntity()->GetPosition().y + (*iter)->GetFRect().y + (*iter)->GetFRect().h) / 30;

			if ((*iter)->GetEntity()->GetSpeed().x < 0.0f)
			{
				unsigned int blockLeft =
					unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + (*iter)->GetEntity()->GetSpeed().x) / 30) % 32;

				for (unsigned int blockY = blockUpY; blockY <= blockDownY; blockY++)
				{
					if (!AdenBlock::GetInstance().canMovingIn[AdenWorldManager::GetInstance().loadedChunks[1]->foreBlocks[blockLeft][blockY]])
					{
						(*iter)->GetEntity()->SetSpeed({ 0.0f, (*iter)->GetEntity()->GetSpeed().y });
						(*iter)->GetEntity()->SetPosition({
							unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + (*iter)->GetEntity()->GetSpeed().x) / 30) * 30
							- (*iter)->GetFRect().x + 1,
							(*iter)->GetEntity()->GetPosition().y
							});
						isHorizonHit = true;
						break;
					}
				}
			}
			else if ((*iter)->GetEntity()->GetSpeed().x > 0.0f)
			{
				unsigned int blockRight =
					unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + (*iter)->GetFRect().w + (*iter)->GetEntity()->GetSpeed().x) / 30) % 32;

				for (unsigned int blockY = blockUpY; blockY <= blockDownY; blockY++)
					if (!AdenBlock::GetInstance().canMovingIn[AdenWorldManager::GetInstance().loadedChunks[1]->foreBlocks[blockRight][blockY]])
					{
						(*iter)->GetEntity()->SetSpeed({ 0, (*iter)->GetEntity()->GetSpeed().y });
						(*iter)->GetEntity()->SetPosition({
							unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + (*iter)->GetFRect().w + (*iter)->GetEntity()->GetSpeed().x) / 30 + 1) * 30
							- ((*iter)->GetFRect().x + (*iter)->GetFRect().w) - 1,
							(*iter)->GetEntity()->GetPosition().y
							});
						isHorizonHit = true;
						break;
					}
			}

			if (!isHorizonHit) (*iter)->GetEntity()->SetPosition({
				(*iter)->GetEntity()->GetPosition().x + (*iter)->GetEntity()->GetSpeed().x,
				(*iter)->GetEntity()->GetPosition().y
				});

			// ������ֱ�ϵ�λ��
			bool isVerticalHit = false;
			unsigned int blockLeftX = unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + 1) / 30) % 32;
			unsigned int blockRightX = unsigned int(((*iter)->GetEntity()->GetPosition().x + (*iter)->GetFRect().x + (*iter)->GetFRect().w - 1) / 30) % 32;

			// ����ֱ��������Ӱ���������ٶȼ����������ٶ�
			if ((*iter)->GetEntity()->GetAffectedByGravity())
			{
				(*iter)->GetEntity()->SetIsFalling(true);
				unsigned int blockUnderFoot = ((*iter)->GetEntity()->GetPosition().y + (*iter)->GetFRect().y + (*iter)->GetFRect().h + 1) / 30;

				for (unsigned int blockX = blockLeftX; blockX <= blockRightX; blockX++)
					if (!AdenBlock::GetInstance().canMovingIn[AdenWorldManager::GetInstance().loadedChunks[1]->foreBlocks[blockX][blockUnderFoot]])
					{
						(*iter)->GetEntity()->SetIsFalling(false);
						break;
					}
				if ((*iter)->GetEntity()->GetIsFalling())
				{
					(*iter)->GetEntity()->SetSpeed({
						(*iter)->GetEntity()->GetSpeed().x,
						(*iter)->GetEntity()->GetSpeed().y + AdenAttribute::GetInstance().fGravityValue
						});
				}
			}

			if ((*iter)->GetEntity()->GetSpeed().y < 0.0f)
			{
				unsigned int blockUp =
					unsigned int(((*iter)->GetEntity()->GetPosition().y + (*iter)->GetFRect().y + (*iter)->GetEntity()->GetSpeed().y) / 30);

				for (unsigned int blockX = blockLeftX; blockX <= blockRightX; blockX++)
					if (!AdenBlock::GetInstance().canMovingIn[AdenWorldManager::GetInstance().loadedChunks[1]->foreBlocks[blockX][blockUp]])
					{
						(*iter)->GetEntity()->SetSpeed({ (*iter)->GetEntity()->GetSpeed().x, 0 });
						(*iter)->GetEntity()->SetPosition({
							(*iter)->GetEntity()->GetPosition().x,
							(blockUp + 1) * 30.0f - (*iter)->GetFRect().y + 1
							});
						isVerticalHit = true;
						break;
					}
			}
			else if ((*iter)->GetEntity()->GetSpeed().y > 0.0f)
			{
				unsigned int blockDown =
					unsigned int(((*iter)->GetEntity()->GetPosition().y + (*iter)->GetFRect().y + (*iter)->GetFRect().h + (*iter)->GetEntity()->GetSpeed().y) / 30);

				for (unsigned int blockX = blockLeftX; blockX <= blockRightX; blockX++)
					if (!AdenBlock::GetInstance().canMovingIn[AdenWorldManager::GetInstance().loadedChunks[1]->foreBlocks[blockX][blockDown]])
					{
						(*iter)->GetEntity()->SetSpeed({ (*iter)->GetEntity()->GetSpeed().x, 0 });
						(*iter)->GetEntity()->SetPosition({
							(*iter)->GetEntity()->GetPosition().x,
							blockDown * 30.0f - ((*iter)->GetFRect().y + (*iter)->GetFRect().h) - 1
							});
						isVerticalHit = true;
						break;
					}
			}
			if (!isVerticalHit)
			{
				(*iter)->GetEntity()->SetPosition({
					(*iter)->GetEntity()->GetPosition().x,
					(*iter)->GetEntity()->GetPosition().y + (*iter)->GetEntity()->GetSpeed().y
					});
			}
		}

		// ����Ⱦ���桿
		AdenRenderer::GetInstance().Draw();

		Uint32 timeFrameEnd = SDL_GetTicks();
		Uint32 timePerFrame = (AdenTimer::GetInstance().currentFrame + 1) * 1000 / AdenWindow::GetInstance().frameAmount;
		if (timeFrameEnd - timeGameStart < timePerFrame)
			SDL_Delay(timePerFrame - (timeFrameEnd - timeGameStart));
		AdenTimer::GetInstance().currentFrame = (AdenTimer::GetInstance().currentFrame + 1) % 4294967296;
		SDL_RenderPresent(AdenWindow::GetInstance().renderer);
	}
}