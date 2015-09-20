#include "Precompiled.h"

#include "Valve.h"
#include "Player.h"
#include "Utils.h"

Valve::Valve( ruSceneNode obj, ruVector3 axis, int turnCount ) {
    mObject = obj;
    mTurnAxis = axis;
    mAngle = 0.0f;
    mDone = false;
    mTurnCount = turnCount;
    mValue = 0.0f;
	mTurnSound = ruSound::Load3D( "data/sounds/valve_squeaky.ogg" );
	mTurnSound.SetPosition( mObject.GetPosition() );
}

void Valve::Reset() {
	mAngle = 0.0f;
	mDone = false;
}

void Valve::Update() {
	mTurnSound.Pause();
    if( !mDone ) {	
		if( pPlayer->mNearestPickedNode == mObject ) {
			pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString( "turnObject" ));
			if( ruIsKeyDown( pPlayer->mKeyUse )) {
				mTurnSound.Play();
				mAngle += 5;
				OnTurn.DoActions();
				if( mAngle >= 360 * mTurnCount ) {
					mDone = true;
					OnTurnDone.DoActions();
				}
				mObject.SetRotation( ruQuaternion( mTurnAxis, mAngle ));
			} 
		}
		mValue = mAngle / (float)( 360 * mTurnCount );
	}
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
