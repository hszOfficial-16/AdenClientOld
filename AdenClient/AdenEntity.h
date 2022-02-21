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
		STATIC = 0,					// 静态图片
		DYNAMIC						// 动态图片
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

	// 设置实体当前为"应当删除"状态，删除任务由管理者完成
	void Destroy() { shouldRelease = true; }
	bool ShouldRelease() { return shouldRelease; }

	// 单例实体管理者
	static std::vector<AdenEntity*>& GetManager() {
		static std::vector<AdenEntity*> instance;
		return instance;
	}

private:
	// 实体当前状态
	double fAngle = 0.0;
	SDL_FPoint fpPosition = { 0.0f, 0.0f };
	SDL_FPoint fpAnchor = { 0.5f, 0.5f };
	SDL_FPoint fpScale = { 1.0f, 1.0f };
	SDL_RendererFlip enumFlip = SDL_FLIP_NONE;

	// 物理相关属性
	bool isFalling = false;
	bool affectedByGravity = false;
	SDL_FPoint fpSpeed = { 0.0f, 0.0f };
	SDL_FPoint fpMaxSpeed = { 0.0f, 0.0f };
	SDL_FPoint fpAcceleration = { 0.0f, 0.0f };
	HorizontalState horizontalState = H_NONE;
	VerticalState verticalState = V_NONE;

	bool shouldRelease = false;

	// 当前贴图信息
	size_t nIndex = 0;
	TextureType textureType = STATIC;
	
	// 贴图向量
	std::vector<AdenImage*> vImage;
	std::vector<AdenAnimation*> vAnimation;
};

class AdenDecisionArea
{
public:
	enum DecisionType
	{
		ATTACKDECISION = 0,					// 攻击判定(仅与受击判定检测)
		BEATENDECISION,						// 受击判定(仅与攻击判定检测)
		COLLISIONBOX						// 碰撞判定(与其他碰撞箱以及方块检测)
	};

	AdenDecisionArea() {}
	AdenDecisionArea(DecisionType, const SDL_FRect&, AdenEntity*);
	~AdenDecisionArea() {}

	SDL_FRect GetFRect() { return frRect; }
	AdenEntity* GetEntity() { return belongTo; }

	void Destroy() { shouldRelease = true; }
	bool ShouldRelease() { return shouldRelease; }

	// 单例判定区管理者
	static std::vector<AdenDecisionArea*>& GetManager(DecisionType type) {
		static std::vector<AdenDecisionArea*> instance[3];
		return instance[type];
	}

private:
	DecisionType enumType = COLLISIONBOX;
	SDL_FRect frRect = { 0, 0, 0, 0 };		// 该判定区的范围(矩形)
	AdenEntity* belongTo = nullptr;			// 该判定区属于的实体

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