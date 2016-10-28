#pragma once

#include "Game.h"

class Actor {
protected:
	float mBodyHeight;
	float mBodyWidth;
	shared_ptr<ruSceneNode> mBody;
	float mSpringLength;
	bool mCrouch;
	float mCrouchMultiplier;
	float mHealth;
	float mMaxHealth;
	float mVerticalSpeed;
	float mLastVerticalPosition;
public:
	explicit Actor(float height, float width);
	virtual ~Actor();
	virtual void SetPosition(ruVector3 position);
	void Move(ruVector3 direction, float speed);

	ruVector3 GetCurrentPosition();
	char IsInsideZone(const shared_ptr<ruSceneNode> & zone);
	void StopInstant();
	void Freeze();
	void Unfreeze();
	bool IsCrouch();
	bool IsVisibleFromPoint(ruVector3 begin);
	void SetBodyVisible(bool state);
	virtual void Damage(float dmg);
	virtual void Heal(float howMuch);
	ruVector3 GetLookDirection();
	float DistanceTo(const shared_ptr<ruSceneNode> & obj) {
		return (obj->GetPosition() - mBody->GetPosition()).Length();
	}
	shared_ptr<ruSceneNode> GetBody() {
		return mBody;
	}
};