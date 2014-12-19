#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift( ruNodeHandle base ) {
    mBaseNode = base;
    mArrived = true;
}

void Lift::Update() {
    if( mBaseNode.IsValid() && mDoorBackLeftNode.IsValid() && mDoorBackRightNode.IsValid() &&
            mDoorFrontLeftNode.IsValid() && mDoorFrontRightNode.IsValid() && mSourceNode.IsValid() &&
            mDestNode.IsValid() && mControlPanel.IsValid() && mTargetNode.IsValid() ) {
        ruVector3 directionVector = ruGetNodePosition( mTargetNode ) - ruGetNodePosition( mBaseNode );
        ruVector3 speedVector = directionVector.Normalized() * 1.2 * g_dt;
        float distSqr = directionVector.Length2();

        if( distSqr < 0.025f ) {
			SetDoorsLocked( false );
            mArrived = true;
        }

        if( distSqr > 1.0f ) {
            distSqr = 1.0f;
        }

        ruSetSoundVolume( mMotorSound, distSqr );

        ruPlaySound( mMotorSound, 1 );
	
        // smooth arriving
        if( !mArrived ) {
            float speed = 1.0f;

            if( distSqr > 1.0f ) {
                speed = 1.0f;
            } else {
                speed = distSqr;
            }
            ruMoveNode( mBaseNode, speedVector * speed );
        }

        // player interaction( TODO: must be moved to player class )
        if( pPlayer->mNearestPickedNode == mControlPanel ) {
            pPlayer->SetActionText( Format( pPlayer->mLocalization.GetString( "liftUpDown" ), GetKeyName( pPlayer->mKeyUse ) ).c_str());

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

void Lift::SetBackDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor ) {
    mDoorBackLeftNode = leftDoor;
    mDoorBackRightNode = rightDoor;
	mDoorBackLeft = new LiftDoor( mDoorBackLeftNode, 90, 0 );
	mDoorBackLeft->SetTurnDirection( Door::TurnDirection::Clockwise );
	mDoorBackRight = new LiftDoor( mDoorBackRightNode, 90, 0 );
	mDoorBackRight->SetTurnDirection( Door::TurnDirection::Counterclockwise );
}

void Lift::SetFrontDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor ) {
    mDoorFrontLeftNode = leftDoor;
    mDoorFrontRightNode = rightDoor;
	mDoorFrontLeft = new LiftDoor( mDoorFrontLeftNode, 90, 0 );
	mDoorFrontLeft->SetTurnDirection( Door::TurnDirection::Counterclockwise );
	mDoorFrontRight = new LiftDoor( mDoorFrontRightNode, 90, 0 );
	mDoorFrontRight->SetTurnDirection( Door::TurnDirection::Clockwise );
}

void Lift::SetMotorSound( ruSoundHandle motorSound ) {
    mMotorSound = motorSound;
    ruAttachSound( mMotorSound, mBaseNode );
}

void Lift::SetSourcePoint( ruNodeHandle sourceNode ) {
    mSourceNode = sourceNode;
    mTargetNode = mSourceNode;
}

void Lift::SetDestinationPoint( ruNodeHandle destNode ) {
    mDestNode = destNode;
}

void Lift::SetControlPanel( ruNodeHandle panel ) {
    mControlPanel = panel;
}

Lift::~Lift() {
	delete mDoorFrontRight;
	delete mDoorFrontLeft;
	delete mDoorBackRight;
	delete mDoorBackLeft;
}

bool Lift::IsArrived() {
    return mArrived;
}

bool Lift::IsAllDoorsClosed()
{
	return mDoorFrontLeft->GetState() == Door::State::Closed &&
		mDoorFrontRight->GetState() == Door::State::Closed && 
		mDoorBackLeft->GetState() == Door::State::Closed &&
		mDoorBackRight->GetState() == Door::State::Closed;
}

void Lift::SetDoorsLocked( bool state )
{
	mDoorFrontLeft->SetLocked( state );
	mDoorFrontRight->SetLocked( state );
	mDoorBackLeft->SetLocked( state );
	mDoorBackRight->SetLocked( state );
}
