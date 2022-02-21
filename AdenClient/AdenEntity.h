#ifndef _ADEN_ENTITY_H_
#define _ADEN_ENTITY_H_

// SDL Support
#include <SDL.h>
#include <SDL_image.h>

// STL Support
#include <vector>
#include <string>
#include <functional>

// Math Support
#include <math.h>

// Module Include
#include "AdenMacro.h"
#include "AdenInstance.h"
#include "AdenGraphic.h"
#include "AdenListener.h"

class AdenEntity
{
public:
	enum EntityType
	{
		PLAYER = 0,
		CREATURE
	};

	enum TextureType
	{
		STATIC = 0,					// ��̬ͼƬ
		DYNAMIC						// ��̬ͼƬ
	};
	enum HorizontalState
	{
		H_NONE = 0,
		LEFT,
		RIGHT
	};
	enum VerticalState
	{
		V_NONE = 0,
		UP,
		DOWN
	};

	AdenEntity(const SDL_FPoint&);
	virtual ~AdenEntity() {}

	// Set Attributes
	void SetPosition(const SDL_FPoint& position) { fpPosition = position; }
	void SetAngle(const double& angle) { fAngle = angle; }
	void SetScale(const SDL_FPoint& scale) { fpScale = scale; }
	void SetAnchor(const SDL_FPoint& anchor) { fpAnchor = anchor; }
	void SetFlip(SDL_RendererFlip flip) { enumFlip = flip; }

	void SetIsFalling(bool whether) { isFalling = whether; }
	void SetAffectedByGravity(bool whether) { affectedByGravity = whether; }
	void SetSpeed(const SDL_FPoint& speed) { fpSpeed = speed; }
	void SetMaxSpeed(const SDL_FPoint& speed) { fpMaxSpeed = speed; }
	void SetAcceleration(const SDL_FPoint& acceleration) { fpAcceleration = acceleration; }
	void SetHorizontalState(HorizontalState hState) { horizontalState = hState; }
	void SetVerticalState(VerticalState vState) { verticalState = vState; }

	// Get Attributes
	const SDL_FPoint& GetPosition() { return fpPosition; }
	const double& GetAngle() { return fAngle; }
	const SDL_FPoint& GetScale() { return fpScale; }
	const SDL_FPoint& GetAnchor() { return fpAnchor; }
	SDL_RendererFlip GetFlip() { return enumFlip; }

	bool GetIsFalling() { return isFalling; }
	bool GetAffectedByGravity() { return affectedByGravity; }
	const SDL_FPoint& GetSpeed() { return fpSpeed; }
	const SDL_FPoint& GetMaxSpeed() { return fpMaxSpeed; }
	const SDL_FPoint& GetAcceleration() { return fpAcceleration; }
	HorizontalState GetHorizontalState() { return horizontalState; }
	VerticalState GetVerticalState() { return verticalState; }

	// Rendering Related Function
	void AddImage(AdenImage* image) { vImage.push_back(image); }
	void AddAnimation(AdenAnimation* animation) { vAnimation.push_back(animation); }

	void Draw(float, float);
	void SetDrawn(TextureType, size_t);

	// ����ʵ�嵱ǰΪ"Ӧ��ɾ��"״̬��ɾ�������ɹ��������
	void Destroy() { shouldRelease = true; }
	bool ShouldRelease() { return shouldRelease; }

	// ����ʵ�������
	static std::vector<AdenEntity*>& GetManager() {
		static std::vector<AdenEntity*> instance;
		return instance;
	}

private:
	// ʵ�嵱ǰ״̬
	double fAngle = 0.0;
	SDL_FPoint fpPosition = { 0.0f, 0.0f };
	SDL_FPoint fpAnchor = { 0.5f, 0.5f };
	SDL_FPoint fpScale = { 1.0f, 1.0f };
	SDL_RendererFlip enumFlip = SDL_FLIP_NONE;

	// �����������
	bool isFalling = false;
	bool affectedByGravity = false;
	SDL_FPoint fpSpeed = { 0.0f, 0.0f };
	SDL_FPoint fpMaxSpeed = { 0.0f, 0.0f };
	SDL_FPoint fpAcceleration = { 0.0f, 0.0f };
	HorizontalState horizontalState = H_NONE;
	VerticalState verticalState = V_NONE;

	bool shouldRelease = false;

	// ��ǰ��ͼ��Ϣ
	size_t nIndex = 0;
	TextureType textureType = STATIC;
	
	// ��ͼ����
	std::vector<AdenImage*> vImage;
	std::vector<AdenAnimation*> vAnimation;
};

class AdenDecisionArea
{
public:
	enum DecisionType
	{
		ATTACKDECISION = 0,					// �����ж�(�����ܻ��ж����)
		BEATENDECISION,						// �ܻ��ж�(���빥���ж����)
		COLLISIONBOX						// ��ײ�ж�(��������ײ���Լ�������)
	};

	AdenDecisionArea() {}
	AdenDecisionArea(DecisionType, const SDL_FRect&, AdenEntity*);
	~AdenDecisionArea() {}

	SDL_FRect GetFRect() { return frRect; }
	AdenEntity* GetEntity() { return belongTo; }

	void Destroy() { shouldRelease = true; }
	bool ShouldRelease() { return shouldRelease; }

	// �����ж���������
	static std::vector<AdenDecisionArea*>& GetManager(DecisionType type) {
		static std::vector<AdenDecisionArea*> instance[3];
		return instance[type];
	}

private:
	DecisionType enumType = COLLISIONBOX;
	SDL_FRect frRect = { 0, 0, 0, 0 };		// ���ж����ķ�Χ(����)
	AdenEntity* belongTo = nullptr;			// ���ж������ڵ�ʵ��

	bool shouldRelease = false;
};

class AdenPlayer : public AdenEntity
{
public:
	enum PlayerType
	{
		SELF = 0,
		OTHER
	};

	AdenPlayer(const SDL_FPoint&, PlayerType);
	virtual ~AdenPlayer();

	void SetNowHealth(const double& health) { nNowHealth = health; }
	void SetMaxHealth(const double& health) { nMaxHealth = health; }

	const double& GetNowHealth() { return nNowHealth; }
	const double& GetMaxHealth() { return nMaxHealth; }

private:
	double nNowHealth;
	double nMaxHealth;

	bool keyHasDown[3] = { false, false, false };

	AdenDecisionArea* decideBeaten;
	AdenDecisionArea* decideCollision;

	AdenSDLListener* listenKeyDown;
	AdenSDLListener* listenKeyUp;

	AdenImage* imageStatic;
	AdenAnimation* aniWalking;

	PlayerType playerType;
};

class AdenCreature : public AdenEntity
{
public:
	enum CreatureType
	{

	};

	AdenCreature(CreatureType, SDL_FPoint, double);
	virtual ~AdenCreature() {}

	void SetNowHealth(double health) { nNowHealth = health; }
	void SetMaxHealth(double health) { nMaxHealth = health; }

	double GetNowHealth() { return nNowHealth; }
	double GetMaxHealth() { return nMaxHealth; }

private:
	double nNowHealth = 0;
	double nMaxHealth = 0;

	CreatureType enumType;
};

#endif // !_ADEN_ENTITY_H_