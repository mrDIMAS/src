#include "Valve.h"
#include "Player.h"
#include "Utils.h"

Valve::Valve( ruNodeHandle obj, ruVector3 axis, int turnCount ) {
    mObject = obj;
    mTurnAxis = axis;
    mAngle = 0.0f;
    mDone = false;
    mTurnCount = turnCount;
    mValue = 0.0f;
}

void Valve::Update() {
    if( mDone ) {
        return;
    }
    if( pPlayer->mNearestPickedNode == mObject ) {
		pPlayer->SetActionText( Format( pPlayer->mLocalization.GetString( "turnObject" ), GetKeyName( pPlayer->mKeyUse )).c_str() );
        if( ruIsKeyDown( pPlayer->mKeyUse )) {
            mAngle += 5;
            if( mAngle >= 360 * mTurnCount ) {
                mDone = true;
            }
            ruSetNodeRotation( mObject, ruQuaternion( mTurnAxis, mAngle ));
        }
    }
    mValue = mAngle / (float)( 360 * mTurnCount );
}

float Valve::GetClosedCoeffecient()
{
	return mValue;
}

bool Valve::IsDone()
{
	return mDone;
}
