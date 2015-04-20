#include "Precompiled.h"

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
	mTurnSound = ruLoadSound3D( "data/sounds/valve_squeaky.ogg" );
	ruSetSoundPosition( mTurnSound, ruGetNodePosition( mObject ));
}

void Valve::Reset() {
	mAngle = 0.0f;
	mDone = false;
}

void Valve::Update() {
	ruPauseSound( mTurnSound );
    if( mDone ) {
        return;
    }
	
    if( pPlayer->mNearestPickedNode == mObject ) {
		pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString( "turnObject" ));
        if( ruIsKeyDown( pPlayer->mKeyUse )) {
			ruPlaySound( mTurnSound );
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

bool Valve::IsPickedByPlayer()
{
	return pPlayer->mNearestPickedNode == mObject;
}
