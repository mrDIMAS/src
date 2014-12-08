#include "Lift.h"
#include "Player.h"
#include "Utils.h"

Lift::Lift( ruNodeHandle object, ruNodeHandle screen, ruNodeHandle src, ruNodeHandle dest, ruSoundHandle motorIS ) {
    body = object;
    scr = screen;
    sourcePoint = src;
    destPoint = dest;
    target = sourcePoint;
    motorIdleSound = motorIS;
    ruAttachSound( motorIdleSound, body );
    arrived = 0;
}

void Lift::Update() {
    ruVector3 delta = ruGetNodePosition( target ) - ruGetNodePosition( body );

    ruVector3 speed =  delta.Normalized() * 1.2 * g_dt;

    float dist2 = delta.Length2();

    if( dist2 < .025 ) {
        arrived = true;
    }

    if( dist2 > 1 ) {
        dist2 = 1;
    }

    ruSetSoundVolume( motorIdleSound, dist2 );

    ruPlaySound( motorIdleSound, 1 );

    if( !arrived ) {
        float mul = 1;

        if( dist2 > 1 ) {
            mul = 1;
        } else {
            mul = dist2;
        }

        ruMoveNode( body, speed * mul );
    }

    if( player->nearestPicked == scr ) {
        ruDrawGUIText( Format( player->localization.GetString( "liftUpDown" ), GetKeyName( player->keyUse )).c_str(), g_resW / 2 - 256, g_resH - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
		
        if( ruIsKeyHit( player->keyUse )) {
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