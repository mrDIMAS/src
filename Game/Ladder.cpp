#include "Ladder.h"
#include "Player.h"

void Ladder::LookAtTarget()
{
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mpCamera->mNode );

    pPlayer->mPitch = atan2f( direction.y, direction.z ) * 180.0f / 3.14159f;
    pPlayer->mPitch = ( pPlayer->mPitch > 0 ? pPlayer->mPitch : ( 360 + pPlayer->mPitch ) );
}

Ladder::Ladder( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint )
{
    freeLook = true;
}

void Ladder::DoPlayerCrawling()
{
    if( !entering )
    {
        ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mBody );

        float distance = direction.Length();

        direction.Normalize();

        if( !leave )
        {
            ruMoveNode( pPlayer->mBody, direction * 0.75f );

            if( distance < 0.25f )
            {
                leave = true;

                if( (ruGetNodePosition( pPlayer->mBody ) - ruGetNodePosition( end )).Length2() < (ruGetNodePosition( pPlayer->mBody ) - ruGetNodePosition( begin )).Length2())
                    target = endLeavePoint;
                else
                    target = beginLeavePoint;
            }
        }
        else
            DoLeave();
    }
}

void Ladder::DoLeave()
{
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mBody );

    float distance = direction.Length();

    direction.Normalize();

    ruMoveNode( pPlayer->mBody, direction * 0.75f );

    if( distance < 0.25f )
    {
        leave = false;

        inside = false;

        pPlayer->SetFootsteps( footsteps );

        ruUnfreeze( pPlayer->mBody );

        ruMoveNode( pPlayer->mBody, ruVector3( 0, 0, 0 ));
    }
}

void Ladder::SetDirection( Direction direction )
{
    if( direction == Way::Direction::Forward )
    {
        if( leave )
            target = endLeavePoint;
        else
            target = end;
    }

    if( direction == Way::Direction::Backward )
    {
        if( leave )
            target = beginLeavePoint;
        else
            target = begin;
    }
}

void Ladder::Enter()
{
    Way::Enter();

    footsteps = pPlayer->mFootstepsType;

    pPlayer->SetFootsteps( FootstepsType::Metal );
}
