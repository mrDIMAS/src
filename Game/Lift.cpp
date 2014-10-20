#include "Lift.h"
#include "Player.h"

Lift::Lift( NodeHandle object, NodeHandle screen, NodeHandle src, NodeHandle dest, SoundHandle motorIS ) {
    body = object;
    scr = screen;
    sourcePoint = src;
    destPoint = dest;
    target = sourcePoint;
    motorIdleSound = motorIS;
    AttachSound( motorIdleSound, body );
    arrived = 0;
}

void Lift::Update() {
    Vector3 delta = GetPosition( target ) - GetPosition( body );

    Vector3 speed =  delta.Normalized() * 1.2;

    float dist2 = delta.Length2();

    if( dist2 < .025 ) {
        arrived = true;
    }

    if( dist2 > 1 ) {
        dist2 = 1;
    }

    SetVolume( motorIdleSound, dist2 );

    PlaySoundSource( motorIdleSound, 1 );

    if( !arrived ) {
        float mul = 1;

        if( dist2 > 1 ) {
            mul = 1;
        } else {
            mul = dist2;
        }

        Move( body, speed * mul );
    }

    if( player->nearestPicked == scr ) {
        DrawGUIText( "[E] - вверх/вниз", g_resW / 2 - 256, g_resH - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

        if( mi::KeyHit( mi::E )) {
            if( arrived ) {
                if( target == sourcePoint ) {
                    target = destPoint;
                } else {
                    target = sourcePoint;
                }
            }

            arrived = false;
        }
    }
}