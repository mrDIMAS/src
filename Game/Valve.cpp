#include "Valve.h"
#include "Player.h"
#include "Utils.h"

Valve::Valve( ruNodeHandle obj, ruVector3 axis, int turnCount )
{
    object = obj;
    tAxis = axis;
    angle = 0;
    done = false;
    tc = turnCount;
    value = 0;
}

void Valve::Update()
{
    if( done )
        return;

    if( pPlayer->mNearestPickedNode == object ) {
        ruDrawGUIText( Format( pPlayer->mLocalization.GetString( "turnObject" ), GetKeyName( pPlayer->mKeyUse )).c_str(), g_resW / 2 - 256, g_resH - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );

        if( ruIsKeyDown( pPlayer->mKeyUse )) {
            angle += 5;

            if( angle >= 360 * tc )
                done = true;

            ruSetNodeRotation( object, ruQuaternion( tAxis, angle ));
        }
    }

    value = angle / (float)( 360 * tc );
}