#include "Ladder.h"
#include "Player.h"

void Ladder::LookAtTarget()
{
    ruVector3 direction = ruGetNodePosition( mTarget ) - ruGetNodePosition( pPlayer->mpCamera->mNode );

    pPlayer->mPitch = atan2f( direction.y, direction.z ) * 180.0f / 3.14159f;
    pPlayer->mPitch = ( pPlayer->mPitch > 0 ? pPlayer->mPitch : ( 360 + pPlayer->mPitch ) );
}

Ladder::Ladder( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint )
{
    mFreeLook = true;
}

void Ladder::DoPlayerCrawling()
{
    if( !mEntering )
    {
        ruVector3 direction = ruGetNodePosition( mTarget ) - pPlayer->GetCurrentPosition();

        float distance = direction.Length();

        direction.Normalize();

        if( !mLeave )
        {
            pPlayer->Move( direction, 0.75f );

            if( distance < 0.25f )
            {
                mLeave = true;

                if( ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mEnd )).Length2() < ( pPlayer->GetCurrentPosition() - ruGetNodePosition( mBegin )).Length2())
                    mTarget = mEndLeavePoint;
                else
                    mTarget = mBeginLeavePoint;
            }
        }
        else
            DoLeave();
    }
}

void Ladder::DoLeave()
{
    ruVector3 direction = ruGetNodePosition( mTarget ) - pPlayer->GetCurrentPosition();

    float distance = direction.Length();

    direction.Normalize();

    pPlayer->Move( direction, 0.75f );

    if( distance < 0.25f )
    {
		mLeave = false;
        mInside = false;
        pPlayer->SetFootsteps( mFootsteps );
        pPlayer->Unfreeze();
        pPlayer->StopInstant();
    }
}

void Ladder::SetDirection( Direction direction )
{
    if( direction == Way::Direction::Forward )
    {
        if( mLeave )
            mTarget = mEndLeavePoint;
        else
            mTarget = mEnd;
    }

    if( direction == Way::Direction::Backward )
    {
        if( mLeave )
            mTarget = mBeginLeavePoint;
        else
            mTarget = mBegin;
    }
}

void Ladder::Enter()
{
    Way::Enter();
    mFootsteps = pPlayer->mFootstepsType;
    pPlayer->SetFootsteps( FootstepsType::Metal );
}
