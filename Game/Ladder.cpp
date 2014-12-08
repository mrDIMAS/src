#include "Ladder.h"
#include "Player.h"

void Ladder::LookAtTarget() {
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->camera->cameraNode );

    player->pitch = atan2f( direction.y, direction.z ) * 180.0f / 3.14159f;
    player->pitch = ( player->pitch > 0 ? player->pitch : ( 360 + player->pitch ) );
}

Ladder::Ladder( ruNodeHandle hBegin, ruNodeHandle hEnd, ruNodeHandle hEnterZone, ruNodeHandle hBeginLeavePoint, ruNodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {
    freeLook = true;
}

void Ladder::DoPlayerCrawling() {
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

void Ladder::DoLeave() {
    ruVector3 direction = ruGetNodePosition( target ) - ruGetNodePosition( player->body );

    float distance = direction.Length();

    direction.Normalize();

    ruMoveNode( player->body, direction * 0.75f );

    if( distance < 0.25f ) {
        leave = false;

        inside = false;

        player->SetFootsteps( footsteps );

        ruUnfreeze( player->body );

        ruMoveNode( player->body, ruVector3( 0, 0, 0 ));
    }
}

void Ladder::SetDirection( Direction direction ) {
    if( direction == Way::Direction::Forward ) {
        if( leave ) {
            target = endLeavePoint;
        } else {
            target = end;
        }
    }

    if( direction == Way::Direction::Backward ) {
        if( leave ) {
            target = beginLeavePoint;
        } else {
            target = begin;
        }
    }
}

void Ladder::Enter() {
    Way::Enter();

    footsteps = player->footstepsType;

    player->SetFootsteps( FootstepsType::Metal );
}
