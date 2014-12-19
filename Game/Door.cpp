#include "Door.h"

#include "Player.h"

vector< Door* > Door::msDoorList;

bool Door::IsPickedByPlayer() {
    return mDoorNode == pPlayer->mNearestPickedNode;
}

Door::~Door() {
    msDoorList.erase( find( msDoorList.begin(), msDoorList.end(), this ));
}

Door::Door( ruNodeHandle hDoor, float fMaxAngle ) {
    mDoorNode = hDoor;
    mMaxAngle = fMaxAngle;
	mLocked = false;
    mOffsetAngle = ruGetNodeEulerAngles( hDoor ).y;
    mCurrentAngle = 0;
    mState = State::Closed;
	SetTurnDirection( TurnDirection::Clockwise );
    mOpenSound = ruLoadSound3D( "data/sounds/door/dooropen.ogg" );
    ruAttachSound( mOpenSound, mDoorNode );
    mCloseSound = ruLoadSound3D( "data/sounds/door/doorclose.ogg" );
    ruAttachSound( mCloseSound, mDoorNode );
    msDoorList.push_back( this );
}

void Door::DoInteraction() {
	float turnSpeed = 60.0f * g_dt;
    if( mState == State::Closing ) {
		if( mTurnDirection == TurnDirection::Clockwise ) {
			mCurrentAngle -= turnSpeed;
			if( mCurrentAngle < 0 ) {
				mCurrentAngle = 0.0f;
				mState = State::Closed;
			}
		} else if( mTurnDirection == TurnDirection::Counterclockwise ){
			mCurrentAngle += turnSpeed;
			if( mCurrentAngle > 0 ) {
				mCurrentAngle = 0;
				mState = State::Closed;
			}
		}

    }

    if( mState == State::Opening ) {
		if( mTurnDirection == TurnDirection::Clockwise ) { 
			mCurrentAngle += turnSpeed;
			if( mCurrentAngle > mMaxAngle ) {
				mState = State::Opened;
				mCurrentAngle = mMaxAngle;
			}
		} else if( mTurnDirection == TurnDirection::Counterclockwise ) {
			mCurrentAngle -= turnSpeed;
			if( mCurrentAngle < -mMaxAngle ) {
				mState = State::Opened;
				mCurrentAngle = -mMaxAngle;
			}
		}

    }

    ruSetNodeRotation( mDoorNode, ruQuaternion( ruVector3( 0, 1, 0 ), mCurrentAngle + mOffsetAngle ));
}

Door::State Door::GetState() {
    return mState;
}

void Door::SwitchState() {
    if( mState == State::Closed ) {
        mState = State::Opening;
        ruPlaySound( mOpenSound );
    }
    if( mState == State::Opened ) {
        mState = State::Closing;
        ruPlaySound( mCloseSound );
    }
}

void Door::Close() {
    if( mState == State::Opened ) {
        mState = State::Closing;
        ruPlaySound( mCloseSound );
    }
}

void Door::Open() {
    if( mState == State::Closed ) {
        mState = State::Opening;
        ruPlaySound( mOpenSound );
    }
}
