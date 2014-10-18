#include "CrawlWay.h"
#include "Player.h"


void CrawlWay::DoPlayerCrawling() {
    if( !entering ) {
        Vector3 direction = GetPosition( target ) - GetPosition( player->body );

        float distance = direction.Length();

        direction.Normalize();

        if( !leave ) {
            Move( player->body, direction * 0.75f );

            if( distance < 0.25f ) {
                leave = true;

                if( (GetPosition( player->body ) - GetPosition( end )).Length2() < (GetPosition( player->body ) - GetPosition( begin )).Length2()) {
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
    Vector3 direction = GetPosition( target ) - GetPosition( player->camera->cameraNode );

    player->yawTo = atan2f( direction.x, direction.z ) * 180.0f / 3.14159f;
    player->yawTo = ( player->yawTo > 0 ? player->yawTo : ( 360 + player->yawTo) );

    player->pitchTo = 15.0f;
}


void CrawlWay::SetDirection( Direction direction ) {

}

CrawlWay::CrawlWay( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone, NodeHandle hBeginLeavePoint, NodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {

}

CrawlWay::~CrawlWay() {

}

void CrawlWay::DoLeave() {
    Vector3 direction = GetPosition( target ) - GetPosition( player->body );

    float distance = direction.Length();

    direction.Normalize();

    Move( player->body, direction * 0.75f );

    if( distance < 0.25f ) {
        leave = false;

        inside = false;

        Unfreeze( player->body );

        Move( player->body, Vector3( 0, 0, 0 ));
    }
}
