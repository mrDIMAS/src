#include "Precompiled.h"

#include "Goal.h"

void Goal::AnimateAndRender() {
    if( ruGetElapsedTimeInSeconds( mWaitTimer ) >= mWaitSec ) {
        mCurrentY += ( mDestY - mCurrentY ) * 0.05f;
    }

    mAlpha = mAlpha + ( 100 - mAlpha ) * 0.05f;

    if( mAlpha <= 101 ) {
        mAlpha = 255.0f;
    }
	ruSetGUINodeAlpha( mGoalText, mAlpha );
    ruSetGUINodePosition( mGoalText, 40, mCurrentY );
}

void Goal::SetText( string t ) {
    ruRestartTimer( mWaitTimer );

    mDestY = mInitialY;

    mText = t;

    ruSetGUINodeText( mGoalText, mText );

    mCurrentY = g_resH * 0.45f;
}

Goal::Goal() {
    mWaitTimer = ruCreateTimer();

    mInitialY = 20;
    mCurrentY = mInitialY;
    mDestY = 20;
    mWaitSec = 2.0f;

    mAlpha = 255.0f;

    mGoalText = ruCreateGUIText( "Goal", 40, mCurrentY, g_resW - 80, 32, pGUI->mFont, ruVector3( 255, 0, 0 ), 1, mAlpha );
}

Goal::~Goal() {
	ruFreeGUINode( mGoalText );
}

void Goal::SetVisible( bool state ) {
    ruSetGUINodeVisible( mGoalText, state );
}
