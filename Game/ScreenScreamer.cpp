#include "ScreenScreamer.h"
#include "Menu.h"
#include "Player.h"

ScreenScreamer * screamer = 0;

ScreenScreamer::ScreenScreamer()
{
    tex = ruGetTexture( "data/textures/effects/noise.jpg" );
    sound = ruLoadSound2D( "data/sounds/screamer.ogg" );

    periods = 0;
    nextPeriodFrames = 0;
    pauseFrames = 0;
}

void ScreenScreamer::DoPeriods( int p )
{
    periods = p;
    nextPeriodFrames = 5 + rand() % 10;
    pauseFrames = 0;
}

void ScreenScreamer::Update()
{
    static int offset = 50;

    if( pMainMenu->IsVisible() ) {
        ruPauseSound( sound );
        return;
    }

    if( periods > 0 ) {
        if( pauseFrames > 0 )
            pauseFrames--;
        else {
            if( nextPeriodFrames > 0 ) {
                int xOff = rand() % offset;
                int yOff = rand() % offset;
                //ruDrawGUIRect( -offset + xOff, -offset + yOff , ruGetResolutionWidth() + xOff, ruGetResolutionHeight() + yOff, tex, ruVector3( 255, 0, 0 ), 120 );

                ruPlaySound( sound );

                pPlayer->DoFright();

                nextPeriodFrames--;
            } else {
                nextPeriodFrames = 5 + rand() % 10;
                pauseFrames = 10 + rand() % 20;
                ruPauseSound( sound );
                periods--;
            }
        }
    }
}
