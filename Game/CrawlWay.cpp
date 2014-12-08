#include "CrawlWay.h"
#include "Player.h"


void CrawlWay::DoPlayerCrawling() {
    if( !entering ) {
        ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->body );

        float distance = direction.Length();

        direction.Normalize();

        if( !leave ) {
            ruMoveNode( player->body, direction * 0.75f );

            if( distance < 0.25f ) {
                leave = true;

                if( (ruGetNodePosition( player->body ) - ruGetNodePosition( end )).Length2() < (ruGetNodePosition( player->body ) - ruGetNodePosition( begin )).Length2()) {
                    target = endLeavePoint;
                } else {
                    target = beginLeavePoint;
                }
            }
        } else {
            DoLeave();
        }
    }
}

void CrawlWay::LookAtTarget() {
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->camera->cameraNode );

    player->yaw = atan2f( direction.x, direction.z ) * 180.0f / 3.14159f;
    player->yaw = ( player->yaw > 0 ? player->yaw : ( 360 + player->yaw ) );

    player->pitch = 15.0f;
}


void CrawlWay::SetDirection( Direction direction ) {

}

CrawlWay::CrawlWay( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {

}

CrawlWay::~CrawlWay() {

}

void CrawlWay::DoLeave() {
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->body );

    float distance = direction.Length();

    direction.Normalize();

    ruMoveNode( player->body, direction * 0.75f );

    if( distance < 0.25f ) {
        leave = false;

        inside = false;

        ruUnfreeze( player->body );

        ruMoveNode( player->body, ruVector3( 0, 0, 0 ));
    }
}
