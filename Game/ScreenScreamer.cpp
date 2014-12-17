#include "ScreenScreamer.h"
#include "Menu.h"
#include "Player.h"

ScreenScreamer * screamer = 0;

ScreenScreamer::ScreenScreamer() {
    mScreamSound = ruLoadSound2D( "data/sounds/screamer.ogg" );
	mGUIFullscreenQuad = ruCreateGUIRect( 0, 0, ruGetResolutionWidth(), ruGetResolutionHeight(),  ruGetTexture( "data/textures/effects/noise.jpg" ), ruVector3( 255, 0, 0 ), 150 );
    mPeriods = 0;
    mNextPeriodFrames = 0;
    mPauseFrames = 0;
	ruSetGUINodeVisible( mGUIFullscreenQuad, false );
}

void ScreenScreamer::DoPeriods( int p ) {
    mPeriods = p;
    mNextPeriodFrames = 5 + rand() % 10;
    mPauseFrames = 0;
}

void ScreenScreamer::Update() {
    static int offset = 50;

    if( pMainMenu->IsVisible() ) {
        ruPauseSound( mScreamSound );
        return;
    }

    if( mPeriods > 0 ) {
        if( mPauseFrames > 0 ) {
			ruSetGUINodeVisible( mGUIFullscreenQuad, false );
            mPauseFrames--;
        } else {
            if( mNextPeriodFrames > 0 ) {
				ruSetGUINodeVisible( mGUIFullscreenQuad, true );
                int xOff = rand() % offset;
                int yOff = rand() % offset;
				ruSetGUINodePosition( mGUIFullscreenQuad, -offset + xOff, -offset + yOff );
				ruSetGUINodeSize( mGUIFullscreenQuad, ruGetResolutionWidth() + xOff, ruGetResolutionHeight() + yOff );
                ruPlaySound( mScreamSound );
                pPlayer->DoFright();
                mNextPeriodFrames--;
            } else {
                mNextPeriodFrames = 5 + rand() % 10;
                mPauseFrames = 10 + rand() % 20;
                ruPauseSound( mScreamSound );
                mPeriods--;
				if( mPeriods <= 0 ){
					ruSetGUINodeVisible( mGUIFullscreenQuad, false );
				}
            }
        }
    }
}
