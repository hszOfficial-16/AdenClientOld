#include "AdenEntity.h"
using namespace std;

AdenEntity::AdenEntity(const SDL_FPoint& point)
	:fpPosition(point)
{
	GetManager().push_back(this);
}

void AdenEntity::SetDrawn(TextureType type, size_t index)
{
	// 如果上一张为动图，则将其设置为初始状况
	if (textureType == DYNAMIC)
		vAnimation[nIndex]->SetDefault();

	textureType = type;
	nIndex = index;
}

void AdenEntity::Draw(float cameraX, float cameraY)
{
	SDL_Rect textureRect; SDL_FRect copyRect; SDL_FPoint copyAnchor;
	switch (textureType)
	{
	case STATIC:
		textureRect = {
			0, 0,
			vImage[nIndex]->GetWidth(),
			vImage[nIndex]->GetHeight()
		};
		copyRect = {
			fpPosition.x - cameraX + AdenWindow::GetInstance().windowWidth / 2 - vImage[nIndex]->GetWidth() * fpScale.x * fpAnchor.x,
			fpPosition.y - cameraY + AdenWindow::GetInstance().windowHeight / 2 - vImage[nIndex]->GetHeight() * fpScale.y * fpAnchor.y,
			vImage[nIndex]->GetWidth() * fpScale.x,
			vImage[nIndex]->GetHeight() * fpScale.y
		};
		copyAnchor =
		{
			vImage[nIndex]->GetWidth() * fpAnchor.x,
			vImage[nIndex]->GetHeight() * fpAnchor.y
		};
		SDL_RenderCopyExF(
			AdenWindow::GetInstance().renderer,
			vImage[nIndex]->GetTexture(),
			&textureRect, &copyRect,
			fAngle, &copyAnchor, enumFlip
		);
		break;

	case DYNAMIC:
		textureRect = {
			0, 0,
			vAnimation[nIndex]->GetFrame().pImage->GetWidth(),
			vAnimation[nIndex]->GetFrame().pImage->GetHeight()
		};
		copyRect = {
			fpPosition.x - cameraX + AdenWindow::GetInstance().windowWidth / 2 - vImage[nIndex]->GetWidth() * fpScale.x * fpAnchor.x,
			fpPosition.y - cameraY + AdenWindow::GetInstance().windowHeight / 2 - vImage[nIndex]->GetHeight() * fpScale.y * fpAnchor.y,
			vAnimation[nIndex]->GetFrame().pImage->GetWidth() * fpScale.x,
			vAnimation[nIndex]->GetFrame().pImage->GetHeight() * fpScale.y
		};
		copyAnchor =
		{
			vAnimation[nIndex]->GetFrame().pImage->GetWidth()* fpAnchor.x,
			vAnimation[nIndex]->GetFrame().pImage->GetHeight()* fpAnchor.y
		};
		SDL_RenderCopyExF(
			AdenWindow::GetInstance().renderer,
			vAnimation[nIndex]->GetFrame().pImage->GetTexture(),
			&textureRect, &copyRect,
			fAngle, &copyAnchor, enumFlip
		);
		vAnimation[nIndex]->OneStep();
		break;

	default:
		break;
	}
}

AdenDecisionArea::AdenDecisionArea(DecisionType type, const SDL_FRect& rect, AdenEntity* pEntity):
	enumType(type), frRect(rect), belongTo(pEntity)
{
	GetManager(type).push_back(this);
}

AdenPlayer::AdenPlayer(const SDL_FPoint& fpPosition, PlayerType type) : AdenEntity(fpPosition),
	playerType(type), nNowHealth(100), nMaxHealth(100)
{
	SetAffectedByGravity(true);
	SetMaxSpeed({ 5.0f, 15.0f });
	SetAcceleration({ 0.2f, 0.0f });

	decideBeaten = new AdenDecisionArea(AdenDecisionArea::BEATENDECISION, { -13, -40, 26, 80 }, this);
	decideCollision = new AdenDecisionArea(AdenDecisionArea::COLLISIONBOX, { -13, -40, 26, 80 }, this);

	imageStatic = new AdenImage("resource//image//Character001.png");
	AddImage(imageStatic);

	aniWalking = new AdenAnimation();
	aniWalking->AddFrameWithPath("resource//image//Character003.png", 5);
	aniWalking->AddFrameWithPath("resource//image//Character001.png", 5);
	aniWalking->AddFrameWithPath("resource//image//Character002.png", 5);
	AddAnimation(aniWalking);

	listenKeyDown = new AdenSDLListener(SDL_KEYDOWN, [this](const SDL_Event& theEvent) {
		switch (theEvent.key.keysym.sym) {
		case SDLK_a:
			if (!keyHasDown[0])
			{
				keyHasDown[0] = true;
				SetFlip(SDL_FLIP_HORIZONTAL);
				SetDrawn(DYNAMIC, 0);
				SetHorizontalState(LEFT);
			}
			break;
		case SDLK_d:
			if (!keyHasDown[1])
			{
				keyHasDown[1] = true;
				SetFlip(SDL_FLIP_NONE);
				SetDrawn(DYNAMIC, 0);
				SetHorizontalState(RIGHT);
			}
			break;
		case SDLK_w:
			if (!keyHasDown[2])
			{
				keyHasDown[2] = true;
				if (!GetIsFalling())
					SetSpeed({ GetSpeed().x, -20.0f });
			}
			break;
		}
		});
	listenKeyUp = new AdenSDLListener(SDL_KEYUP, [this](const SDL_Event& theEvent) {
		switch (theEvent.key.keysym.sym) {
		case SDLK_a:
			if (keyHasDown[0])
			{
				keyHasDown[0] = false;
				if (!keyHasDown[1])
				{
					SetDrawn(STATIC, 0);
					SetHorizontalState(H_NONE);
				}
			}
			break;
		case SDLK_d:
			if (keyHasDown[1])
			{
				keyHasDown[1] = false;
				if (!keyHasDown[0])
				{
					SetDrawn(STATIC, 0);
					SetHorizontalState(H_NONE);
				}
			}
			break;
		case SDLK_w:
			if (keyHasDown[2])
				keyHasDown[2] = false;
		}
		});
}

AdenPlayer::~AdenPlayer()
{
	decideBeaten->Destroy();
	decideCollision->Destroy();

	listenKeyDown->Destroy();
	listenKeyUp->Destroy();

	delete imageStatic;
	delete aniWalking;
}

AdenCreature::AdenCreature(CreatureType type, SDL_FPoint point, double health):
	AdenEntity(point), enumType(type), nMaxHealth(health), nNowHealth(health)
{

}