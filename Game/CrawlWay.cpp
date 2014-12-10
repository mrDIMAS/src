#include "CrawlWay.h"
#include "Player.h"


void CrawlWay::DoPlayerCrawling()
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

void CrawlWay::LookAtTarget()
{
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mpCamera->mNode );

    pPlayer->mYaw = atan2f( direction.x, direction.z ) * 180.0f / 3.14159f;
    pPlayer->mYaw = ( pPlayer->mYaw > 0 ? pPlayer->mYaw : ( 360 + pPlayer->mYaw ) );

    pPlayer->mPitch = 15.0f;
}


void CrawlWay::SetDirection( Direction direction )
{

}

CrawlWay::CrawlWay( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint )
{

}

CrawlWay::~CrawlWay()
{

}

void CrawlWay::DoLeave()
{
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( pPlayer->mBody );

    float distance = direction.Length();

    direction.Normalize();

    ruMoveNode( pPlayer->mBody, direction * 0.75f );

    if( distance < 0.25f )
    {
        leave = false;

        inside = false;

        ruUnfreeze( pPlayer->mBody );

        ruMoveNode( pPlayer->mBody, ruVector3( 0, 0, 0 ));
    }
}
