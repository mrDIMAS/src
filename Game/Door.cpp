#include "Door.h"

#include "Player.h"

vector< Door* > Door::msDoorList;

bool Door::IsPickedByPlayer()
{
    return door == pPlayer->mNearestPickedNode;
}

Door::~Door()
{
    msDoorList.erase( find( msDoorList.begin(), msDoorList.end(), this ));
}

Door::Door( ruNodeHandle hDoor, float fMaxAngle )
{
    door = hDoor;

    mMaxAngle = fMaxAngle;

    mOffsetAngle = ruGetNodeEulerAngles( hDoor ).y;

    mCurrentAngle = 0;

    mState = State::Closed;

    mOpenSound = ruLoadSound3D( "data/sounds/door/dooropen.ogg" );
    ruAttachSound( mOpenSound, door );

    mCloseSound = ruLoadSound3D( "data/sounds/door/doorclose.ogg" );
    ruAttachSound( mCloseSound, door );

    msDoorList.push_back( this );
}

void Door::DoInteraction()
{
    if( mState == State::Closing )
    {
        mCurrentAngle -= 60.0f * g_dt;

        if( mCurrentAngle < 0 )
        {
            mCurrentAngle = 0.0f;

            mState = State::Closed;
        }
    }

    if( mState == State::Opening )
    {
        mCurrentAngle += 60.0f * g_dt;

        if( mCurrentAngle > mMaxAngle )
        {
            mState = State::Opened;

            mCurrentAngle = mMaxAngle;
        }
    }

    ruSetNodeRotation( door, ruQuaternion( ruVector3( 0, 1, 0 ), mCurrentAngle + mOffsetAngle ));
}

Door::State Door::GetState()
{
    return mState;
}

void Door::SwitchState()
{
    if( mState == State::Closed )
    {
        mState = State::Opening;
        ruPlaySound( mOpenSound );
    }
    if( mState == State::Opened )
    {
        mState = State::Closing;
        ruPlaySound( mCloseSound );
    }
}

void Door::Close()
{
    if( mState == State::Opened )
    {
        mState = State::Closing;
        ruPlaySound( mCloseSound );
    }
}

void Door::Open()
{
    if( mState == State::Closed )
    {
        mState = State::Opening;
        ruPlaySound( mOpenSound );
    }
}
