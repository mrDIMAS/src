#include "Precompiled.h"
#include "Level.h"
#include "Valve.h"
#include "Player.h"
#include "Utils.h"

Valve::Valve( shared_ptr<ruSceneNode> obj, ruVector3 axis, int turnCount ) {
    mObject = obj;
    mTurnAxis = axis;
    mAngle = 0.0f;
    mDone = false;
    mTurnCount = turnCount;
    mValue = 0.0f;
	mTurnSound = ruSound::Load3D( "data/sounds/valve_squeaky.ogg" );
	mTurnSound->SetPosition( mObject->GetPosition() );
}

void Valve::Reset() {
	mAngle = 0.0f;
	mDone = false;
}

void Valve::Update() {
	mTurnSound->Pause();
    if( !mDone ) {	
		auto & player = Level::Current()->GetPlayer();
		if(player->mNearestPickedNode == mObject ) {
			player->GetHUD()->SetAction(player->mKeyUse, player->mLocalization.GetString( "turnObject" ));
			if( ruInput::IsKeyDown(player->mKeyUse )) {
				mTurnSound->Play();
				mAngle += 5;
				OnTurn();
				if( mAngle >= 360 * mTurnCount ) {
					mDone = true;
					OnTurnDone();
				}
				mObject->SetRotation( ruQuaternion( mTurnAxis, mAngle ));
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
	auto & player = Level::Current()->GetPlayer();
	return player->mNearestPickedNode == mObject;
}
