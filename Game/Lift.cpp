#include "Precompiled.h"

#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift( ruSceneNode base ) : mPaused( false ), mBaseNode( base ), mArrived( true ), mEngineSoundEnabled( true ), mSpeedMultiplier( 1.0f ) {

}

void Lift::Update() {
    if( mBaseNode.IsValid() && mDoorBackLeftNode.IsValid() && mDoorBackRightNode.IsValid() &&
            mDoorFrontLeftNode.IsValid() && mDoorFrontRightNode.IsValid() && mSourceNode.IsValid() &&
            mDestNode.IsValid() && mControlPanel.IsValid() && mTargetNode.IsValid() ) {

        ruVector3 directionVector = mTargetNode.GetPosition() - mBaseNode.GetPosition();
        ruVector3 speedVector = directionVector.Normalized() * 1.2 * g_dt;
        float distSqr = directionVector.Length2();

        if( distSqr < 0.025f ) {
			SetDoorsLocked( false );
            mArrived = true;
        }

        if( distSqr > 1.0f ) {
            distSqr = 1.0f;
        }

        mMotorSound.SetVolume( distSqr );

		if( mEngineSoundEnabled ) {
			mMotorSound.Play();
		} else {
			mMotorSound.Pause();
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
				mBaseNode.Move( speedVector * speed );
			} else {
				mMotorSound.Pause();
			}
        }
        // player interaction( TODO: must be moved to player class )
        if( pPlayer->mNearestPickedNode == mControlPanel ) {
            pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString( "liftUpDown" ) );

            if( ruIsKeyHit( pPlayer->mKeyUse )) {
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
        RaiseError( "Lift objects are set improperly!" );
    }
}

void Lift::SetBackDoors( ruSceneNode leftDoor, ruSceneNode rightDoor ) {
    mDoorBackLeftNode = leftDoor;
    mDoorBackRightNode = rightDoor;
	mDoorBackLeft = unique_ptr<LiftDoor>( new LiftDoor( mDoorBackLeftNode, -90, 0 ));
	mDoorBackLeft->SetTurnDirection( Door::TurnDirection::Clockwise );
	mDoorBackRight = unique_ptr<LiftDoor>( new LiftDoor( mDoorBackRightNode, -90, 0 ));
	mDoorBackRight->SetTurnDirection( Door::TurnDirection::Counterclockwise );
}

void Lift::SetFrontDoors( ruSceneNode leftDoor, ruSceneNode rightDoor ) {
    mDoorFrontLeftNode = leftDoor;
    mDoorFrontRightNode = rightDoor;
	mDoorFrontLeft = unique_ptr<LiftDoor>( new LiftDoor( mDoorFrontLeftNode, 90, 0 ));
	mDoorFrontLeft->SetTurnDirection( Door::TurnDirection::Counterclockwise );
	mDoorFrontRight = unique_ptr<LiftDoor>( new LiftDoor( mDoorFrontRightNode, 90, 0 ));
	mDoorFrontRight->SetTurnDirection( Door::TurnDirection::Clockwise );
}

void Lift::SetMotorSound( ruSound motorSound ) {
    mMotorSound = motorSound;
    mMotorSound.Attach( mBaseNode );
}

void Lift::SetSourcePoint( ruSceneNode sourceNode ) {
    mSourceNode = sourceNode;
    mTargetNode = mSourceNode;
}

void Lift::SetDestinationPoint( ruSceneNode destNode ) {
    mDestNode = destNode;
}

void Lift::SetControlPanel( ruSceneNode panel ) {
    mControlPanel = panel;
}

Lift::~Lift() {
	mMotorSound.Free();
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
