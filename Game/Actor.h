#pragma once

#include "Game.h"

class Actor {
protected:
    float mBodyHeight;
    float mBodyWidth;
    ruNodeHandle mBody;
	float mSpringLength;
	bool mCrouch;
	float mCrouchMultiplier;
public:
    explicit Actor( float height, float width );
    virtual ~Actor();
    void SetPosition( ruVector3 position );
    void Move( ruVector3 direction, float speed );
	void Step( ruVector3 direction, float speed );
    ruVector3 GetCurrentPosition( );
    char IsInsideZone( ruNodeHandle zone );
    void StopInstant();
    void Freeze();
    void Unfreeze();
	void Crouch( bool state );
	void UpdateCrouch();
	bool IsCrouch();
    bool IsVisibleFromPoint( ruVector3 begin );
	void SetBodyVisible( bool state );
    ruVector3 GetLookDirection();
};