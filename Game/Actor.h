#pragma once

#include "Game.h"

class Actor {
protected:
    float mBodyHeight;
    float mBodyWidth;
    ruNodeHandle mBody;
public:

    explicit Actor( float height, float width );
    virtual ~Actor();
    void SetPosition( ruVector3 position );
    void Move( ruVector3 direction, float speed );
    ruVector3 GetCurrentPosition( );
    char IsInsideZone( ruNodeHandle zone );
    void StopInstant();
    void Freeze();
    void Unfreeze();
    bool IsVisibleFromPoint( ruVector3 begin ) {
        return ruCastRay( begin, GetCurrentPosition(), nullptr ).pointer == mBody.pointer;
    }
	void SetBodyVisible( bool state ) {
		if( state ) {
			ruShowNode( mBody );
		} else {
			ruHideNode( mBody );
		}
	}
    ruVector3 GetLookDirection();
};