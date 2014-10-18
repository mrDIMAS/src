#include "Goal.h"




void Goal::AnimateAndRender() {
    if( GetElapsedTimeInSeconds( waitTimer ) >= waitSec ) {
        currentY += ( destY - currentY ) * 0.05f;
    }

    alpha = alpha + ( 100 - alpha ) * 0.05f;

    if( alpha <= 101 ) {
        alpha = 255.0f;
    }

    DrawGUIText( text.c_str(), 40, currentY, GetResolutionWidth() - 80, 32, gui->font, Vector3( 255, 0, 0 ), 1, alpha );
}

void Goal::SetText( string t ) {
    RestartTimer( waitTimer );

    destY = initialY;

    text = t;

    currentY = GetResolutionHeight() * 0.45f;
}

Goal::Goal() {
    waitTimer = CreateTimer();

    initialY = 20;
    currentY = initialY;
    destY = 20;
    waitSec = 2.0f;

    alpha = 255.0f;
}
