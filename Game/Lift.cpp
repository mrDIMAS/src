#include "Precompiled.h"
#include "Level.h"
#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift( shared_ptr<ruSceneNode> base ) : mPaused( false ), mBaseNode( base ), mArrived( true ), mEngineSoundEnabled( true ), mSpeedMultiplier( 1.0f ) {
	mMotorSound = ruSound::Load3D( "data/sounds/motor_idle.ogg");
	mMotorSound->Attach( mBaseNode );
	mMotorSound->SetRolloffFactor( 30 );
	mMotorSound->SetRoomRolloffFactor( 30 );
}

void Lift::Update() {
    if( mBaseNode && mSourceNode && mDestNode && mControlPanel && mTargetNode ) {

        ruVector3 directionVector = mTargetNode->GetPosition() - mBaseNode->GetPosition();
        ruVector3 speedVector = directionVector.Normalized() * 0.02f;
        float distSqr = directionVector.Length2();

        if( distSqr < 0.025f ) {
			SetDoorsLocked( false );
            mArrived = true;
        }

        if( distSqr > 1.0f ) {
            distSqr = 1.0f;
        }

        mMotorSound->SetVolume( distSqr );

		if( mEngineSoundEnabled ) {
			mMotorSound->Play();
		} else {
			mMotorSound->Pause();
		}
	
        // smooth arriving
        if( !mArrived ) {
            float speed = 1.0f;

            if( distSqr > 1.0f ) {
                speed = 1.0f;
            } else {
                speed = distSqr;
            }
			
			speed *= mSpeedMultiplier;

			if( !mPaused ) {
				mBaseNode->Move( speedVector * speed );
			} else {
				mMotorSound->Pause();
			}
        }
        // player interaction( TODO: must be moved to player class )
		auto & player = Level::Current()->GetPlayer();
        if(player->mNearestPickedNode == mControlPanel ) {
			player->SetActionText( StringBuilder() << ruInput::GetKeyName(player->mKeyUse ) << player->mLocalization.GetString( "liftUpDown" ) );

            if( ruInput::IsKeyHit(player->mKeyUse )) {
                if( mArrived ) {
					if( IsAllDoorsClosed() ) {
						SetDoorsLocked( true );
						if( mTargetNode == mSourceNode ) {
							mTargetNode = mDestNode;
						} else {
							mTargetNode = mSourceNode;
						}
					}
                }

                mArrived = false;
            }
        }
    } else {
        throw std::runtime_error( "Lift objects are set improperly!" );
    }
}

void Lift::SetBackDoors( const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor ) {
	mDoorBackLeft = leftDoor;
	mDoorBackLeft->SetTurnDirection( Door::TurnDirection::Clockwise );
	mDoorBackRight = rightDoor;
	mDoorBackRight->SetTurnDirection( Door::TurnDirection::Counterclockwise );
}

void Lift::SetFrontDoors( const shared_ptr<Door> & leftDoor, const shared_ptr<Door> & rightDoor ) {
	mDoorFrontLeft = leftDoor;
	mDoorFrontLeft->SetTurnDirection( Door::TurnDirection::Counterclockwise );
	mDoorFrontRight = rightDoor;
	mDoorFrontRight->SetTurnDirection( Door::TurnDirection::Clockwise );
}

void Lift::SetSourcePoint( shared_ptr<ruSceneNode> sourceNode ) {
    mSourceNode = sourceNode;
    mTargetNode = mSourceNode;
}

void Lift::SetDestinationPoint( shared_ptr<ruSceneNode> destNode ) {
    mDestNode = destNode;
}

void Lift::SetControlPanel( shared_ptr<ruSceneNode> panel ) {
    mControlPanel = panel;
}

Lift::~Lift() {

}

bool Lift::IsArrived() {
    return mArrived;
}

bool Lift::IsAllDoorsClosed() {
	return mDoorFrontLeft->GetState() == Door::State::Closed &&
		mDoorFrontRight->GetState() == Door::State::Closed && 
		mDoorBackLeft->GetState() == Door::State::Closed &&
		mDoorBackRight->GetState() == Door::State::Closed;
}

void Lift::SetDoorsLocked( bool state ) {
	mDoorFrontLeft->SetLocked( state );
	mDoorFrontRight->SetLocked( state );
	mDoorBackLeft->SetLocked( state );
	mDoorBackRight->SetLocked( state );
}

void Lift::SetPaused( bool state ) {
	mPaused = state;
}

void Lift::SetEngineSoundEnabled( bool state ) {
	mEngineSoundEnabled = state;
}

void Lift::SetSpeedMultiplier( float mult ) {
	mSpeedMultiplier = mult;
}

void Lift::Serialize( SaveFile & out ) {
	out.WriteBoolean( mArrived );
	out.WriteBoolean( mPaused );
	out.WriteBoolean( mEngineSoundEnabled );
	out.WriteFloat( mSpeedMultiplier );
	if( mTargetNode == mSourceNode ) {
		out.WriteInteger( 0 );
	} else {
		out.WriteInteger( 1 );
	}
}

void Lift::Deserialize( SaveFile & in ) {
	in.ReadBoolean( mArrived );
	in.ReadBoolean( mPaused );
	in.ReadBoolean( mEngineSoundEnabled );
	in.ReadFloat( mSpeedMultiplier );
	if( in.ReadInteger() == 0 ) {
		mTargetNode = mSourceNode;
	} else {
		mTargetNode = mDestNode;
	}
}
