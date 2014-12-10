#include "Goal.h"




void Goal::AnimateAndRender()
{
    if( ruGetElapsedTimeInSeconds( waitTimer ) >= waitSec )
        currentY += ( destY - currentY ) * 0.05f;

    alpha = alpha + ( 100 - alpha ) * 0.05f;

    if( alpha <= 101 )
        alpha = 255.0f;

    ruDrawGUIText( text.c_str(), 40, currentY, g_resW - 80, 32, pGUI->mFont, ruVector3( 255, 0, 0 ), 1, alpha );
}

void Goal::SetText( string t )
{
    ruRestartTimer( waitTimer );

    destY = initialY;

    text = t;

    currentY = g_resH * 0.45f;
}

Goal::Goal()
{
    waitTimer = ruCreateTimer();

    initialY = 20;
    currentY = initialY;
    destY = 20;
    waitSec = 2.0f;

    alpha = 255.0f;
}
