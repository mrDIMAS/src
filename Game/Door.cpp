#include "Precompiled.h"

#include "Door.h"
#include "Player.h"

vector< Door* > Door::msDoorList;

bool Door::IsPickedByPlayer() {
    return mDoorNode == pPlayer->mNearestPickedNode;
}

Door::~Door() {
	mOpenSound.Free();
	mCloseSound.Free();
    msDoorList.erase( find( msDoorList.begin(), msDoorList.end(), this ));
}

Door::Door( ruSceneNode hDoor, float fMaxAngle ) : mDoorNode( hDoor ),
													mMaxAngle( fMaxAngle ),
													mLocked( false ),
													mCurrentAngle( 0.0f ),
													mState( State::Closed )
{
    mOffsetAngle = hDoor.GetEulerAngles().y;
	SetTurnDirection( TurnDirection::Clockwise );
    mOpenSound = ruSound::Load3D( "data/sounds/door/dooropen.ogg" );
    mOpenSound.Attach( mDoorNode );
    mCloseSound = ruSound::Load3D( "data/sounds/door/doorclose.ogg" );
    mCloseSound.Attach( mDoorNode );
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

    mDoorNode.SetRotation( ruQuaternion( ruVector3( 0, 1, 0 ), mCurrentAngle + mOffsetAngle ));
}

Door::State Door::GetState() {
    return mState;
}

void Door::SwitchState() {
    if( mState == State::Closed ) {
        mState = State::Opening;
        mOpenSound.Play();
    }
    if( mState == State::Opened ) {
        mState = State::Closing;
        mCloseSound.Play();
    }
}

void Door::Close() {
    if( mState == State::Opened ) {
        mState = State::Closing;
        mCloseSound.Play();
    }
}

void Door::Open() {
    if( mState == State::Closed ) {
        mState = State::Opening;
        mOpenSound.Play();
    }
}

Door * Door::GetByName( const string & name ) {
	for( auto pDoor : msDoorList ) {
		if( pDoor->mDoorNode.GetName() == name ) {
			return pDoor;
		}
	}
	return nullptr;
}

void Door::Deserialize( SaveFile & in ) {
	in.ReadFloat( mMaxAngle );
	in.ReadFloat( mOffsetAngle );
	in.ReadFloat( mCurrentAngle );
	mTurnDirection = (TurnDirection)in.ReadInteger();
	mState = (State)in.ReadInteger();
	in.ReadBoolean( mLocked );
}

void Door::Serialize( SaveFile & out ) {
	out.WriteFloat( mMaxAngle );
	out.WriteFloat( mOffsetAngle );
	out.WriteFloat( mCurrentAngle );
	out.WriteInteger( (int)mTurnDirection );
	out.WriteInteger( (int)mState );
	out.WriteBoolean( mLocked );
}

void Door::SetLocked( bool state ) {
	mLocked = state;
}

bool Door::IsLocked() {
	return mLocked;
}

void Door::SetTurnDirection( TurnDirection direction ) {
	mTurnDirection = direction;
}
