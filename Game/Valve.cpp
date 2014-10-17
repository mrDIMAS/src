#include "Valve.h"
#include "Player.h"

Valve::Valve( NodeHandle obj, Vector3 axis, int turnCount ) {
    object = obj;
    tAxis = axis;
    angle = 0;
    done = false;
    tc = turnCount;
    value = 0;
}

void Valve::Update() {
    if( done )
        return;

    if( player->nearestPicked == object ) {
        DrawGUIText( "[E] - вращать", GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

        if( mi::KeyDown( mi::E )) {
            angle += 5;

            if( angle >= 360 * tc )
                done = true;

            SetRotation( object, Quaternion( tAxis, angle ));
        }
    }

    value = angle / (float)( 360 * tc );
}