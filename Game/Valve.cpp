#include "Valve.h"
#include "Player.h"
#include "Utils.h"

Valve::Valve( ruNodeHandle obj, ruVector3 axis, int turnCount ) {
    object = obj;
    tAxis = axis;
    angle = 0;
    done = false;
    tc = turnCount;
    value = 0;
}

void Valve::Update() {
    if( done ) {
        return;
    }

    if( player->nearestPicked == object ) {
        ruDrawGUIText( Format( player->localization.GetString( "turnObject" ), GetKeyName( player->keyUse )).c_str(), g_resW / 2 - 256, g_resH - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );

        if( ruIsKeyDown( player->keyUse )) {
            angle += 5;

            if( angle >= 360 * tc ) {
                done = true;
            }

            ruSetNodeRotation( object, ruQuaternion( tAxis, angle ));
        }
    }

    value = angle / (float)( 360 * tc );
}