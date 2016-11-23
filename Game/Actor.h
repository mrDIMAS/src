#pragma once

#include "Game.h"

class Actor {
protected:
	unique_ptr<Game> & mGame;
	float mBodyHeight;
	float mBodyWidth;
	shared_ptr<ISceneNode> mBody;
	float mSpringLength;
	bool mCrouch;
	float mCrouchMultiplier;
	float mHealth;
	float mMaxHealth;
	float mVerticalSpeed;
	float mLastVerticalPosition;
public:
	explicit Actor(unique_ptr<Game> & game, float height, float width);
	virtual ~Actor();
	virtual void SetPosition(Vector3 position);
	void Move(Vector3 direction, float speed);
	unique_ptr<Game> & GetGame() const {
		return mGame;
	}
	Vector3 GetCurrentPosition();
	char IsInsideZone(const shared_ptr<ISceneNode> & zone);
	void StopInstantly();
	void Freeze();
	void Unfreeze();
	bool IsCrouch();
	bool IsVisibleFromPoint(Vector3 begin);
	void SetBodyVisible(bool state);
	virtual void Damage(float dmg);
	virtual void Heal(float howMuch);
	Vector3 GetLookDirection();
	float DistanceTo(const shared_ptr<ISceneNode> & obj)
	{
		return (obj->GetPosition() - mBody->GetPosition()).Length();
	}
	shared_ptr<ISceneNode> GetBody()
	{
		return mBody;
	}
};