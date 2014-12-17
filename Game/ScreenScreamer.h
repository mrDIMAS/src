#pragma  once

#include "Game.h"

class ScreenScreamer {
private:
	ruSoundHandle mScreamSound;
	int mPeriods;
	int mNextPeriodFrames;
	int mPauseFrames;
	ruRectHandle mGUIFullscreenQuad;
public:
    explicit ScreenScreamer();
    void DoPeriods( int p );
    void Update( );
	void SetVisible( bool state ){
		if( !state ) {
			ruSetGUINodeVisible( mGUIFullscreenQuad, state );
		}
	}
};