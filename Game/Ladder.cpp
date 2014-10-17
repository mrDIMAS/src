#include "Ladder.h"
#include "Player.h"

void Ladder::LookAtTarget() {
    Vector3 direction = GetPosition( target ) - GetPosition( player->camera->cameraNode );

    player->pitchTo = atan2f( direction.y, direction.z ) * 180.0f / 3.14159f;
    player->pitchTo = ( player->pitchTo > 0 ? player->pitchTo : ( 360 + player->pitchTo) );
}

Ladder::Ladder( NodeHandle hBegin, NodeHandle hEnd, NodeHandle hEnterZone, NodeHandle hBeginLeavePoint, NodeHandle hEndLeavePoint )
    : Way( hBegin, hEnd, hEnterZone, hBeginLeavePoint, hEndLeavePoint ) {
    freeLook = true;
}

void Ladder::DoPlayerCrawling() {
    if( !entering ) {
        Vector3 direction = GetPosition( target ) - GetPosition( player->body );

        float distance = direction.Length();

        direction.Normalize();

        if( !leave ) {
            Move( player->body, direction * 0.75f );

            if( distance < 0.25f ) {
                leave = true;

                if( (GetPosition( player->body ) - GetPosition( end )).Length2() < (GetPosition( player->body ) - GetPosition( begin )).Length2())
                    target = endLeavePoint;
                else
                    target = beginLeavePoint;
            }
        }
        else {
            DoLeave();
        }
    }
}

void Ladder::DoLeave() {
    Vector3 direction = GetPosition( target ) - GetPosition( player->body );

    float distance = direction.Length();

    direction.Normalize();

    Move( player->body, direction * 0.75f );

    if( distance < 0.25f ) {
        leave = false;

        inside = false;

        player->SetFootsteps( footsteps );

        Unfreeze( player->body );

        Move( player->body, Vector3( 0, 0, 0 ));
    }
}

void Ladder::SetDirection( Direction direction ) {
    if( direction == Way::Direction::Forward ) {
        if( leave )
            target = endLeavePoint;
        else
            target = end;
    }

    if( direction == Way::Direction::Backward ) {
        if( leave )
            target = beginLeavePoint;
        else
            target = begin;
    }
}

void Ladder::Enter() {
    Way::Enter();

    footsteps = player->footstepsType;

    player->SetFootsteps( FootstepsType::Metal );
}
