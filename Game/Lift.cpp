#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift( ruNodeHandle base )
{
    mBaseNode = base;
    mArrived = true;
}

void Lift::Update()
{
	if( mBaseNode.IsValid() && mDoorBackLeft.IsValid() && mDoorBackRight.IsValid() && 
		mDoorFrontLeft.IsValid() && mDoorFrontRight.IsValid() && mSourceNode.IsValid() &&
		mDestNode.IsValid() && mControlPanel.IsValid() && mTargetNode.IsValid() ) 
	{
		ruVector3 directionVector = ruGetNodePosition( mTargetNode ) - ruGetNodePosition( mBaseNode );
		ruVector3 speedVector = directionVector.Normalized() * 1.2 * g_dt;
		float distSqr = directionVector.Length2();

		if( distSqr < 0.025f )
			mArrived = true;

		if( distSqr > 1.0f )
			distSqr = 1.0f;

		ruSetSoundVolume( mMotorSound, distSqr );

		ruPlaySound( mMotorSound, 1 );

		// smooth arriving
		if( !mArrived ) 
		{
			float speed = 1.0f;

			if( distSqr > 1.0f )
				speed = 1.0f;
			else
				speed = distSqr;
			ruMoveNode( mBaseNode, speedVector * speed );
		}

		// player interaction( TODO: must be moved to player class )
		if( pPlayer->mNearestPickedNode == mControlPanel ) 
		{
			ruDrawGUIText( Format( pPlayer->mLocalization.GetString( "liftUpDown" ), GetKeyName( pPlayer->mKeyUse ) ).c_str(), g_resW / 2 - 256, g_resH - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );

			if( ruIsKeyHit( pPlayer->mKeyUse )) 
			{
				if( mArrived ) 
				{
					if( mTargetNode == mSourceNode )
						mTargetNode = mDestNode;
					else
						mTargetNode = mSourceNode;
				}

				mArrived = false;
			}
		}
	} else {
		RaiseError( "Lift object are set improperly!" );
	}
}

void Lift::SetBackDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor )
{
	mDoorBackLeft = leftDoor;
	mDoorBackRight = rightDoor;
}

void Lift::SetFrontDoors( ruNodeHandle leftDoor, ruNodeHandle rightDoor )
{
	mDoorFrontLeft = leftDoor;
	mDoorFrontRight = rightDoor;
}

void Lift::SetMotorSound( ruSoundHandle motorSound )
{
	mMotorSound = motorSound;
	ruAttachSound( mMotorSound, mBaseNode );
}

void Lift::SetSourcePoint( ruNodeHandle sourceNode )
{
	mSourceNode = sourceNode;
	mTargetNode = mSourceNode;
}

void Lift::SetDestinationPoint( ruNodeHandle destNode )
{
	mDestNode = destNode;
}

void Lift::SetControlPanel( ruNodeHandle panel )
{
	mControlPanel = panel;
}

Lift::~Lift()
{

}

bool Lift::IsArrived()
{
	return mArrived;
}
