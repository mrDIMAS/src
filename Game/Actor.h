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
    explicit Actor( float height, float width );
    virtual ~Actor();
    virtual void SetPosition( ruVector3 position );
    void Move( ruVector3 direction, float speed );
	
    ruVector3 GetCurrentPosition( );
    char IsInsideZone( shared_ptr<ruSceneNode> zone );
    void StopInstant();
    void Freeze();
    void Unfreeze();
	void Crouch( bool state );
	void UpdateCrouch();
	bool IsCrouch();
    bool IsVisibleFromPoint( ruVector3 begin );
	void SetBodyVisible( bool state );
	virtual void Damage( float dmg );
	virtual void Heal( float howMuch );
    ruVector3 GetLookDirection();
};