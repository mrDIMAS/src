#include "Precompiled.h"

#include "Goal.h"

void Goal::AnimateAndRender() {
    if( mWaitTimer->GetElapsedTimeInSeconds() >= mWaitSec ) {
        mCurrentY += ( mDestY - mCurrentY ) * 0.05f;
    }

    mAlpha = mAlpha + ( 100 - mAlpha ) * 0.05f;

    if( mAlpha <= 101 ) {
        mAlpha = 255.0f;
    }
	mGoalText->SetAlpha( mAlpha );
    mGoalText->SetPosition( 40, mCurrentY );
}

void Goal::SetText( string t ) {
    mWaitTimer->Restart();
    mDestY = mInitialY;
    mText = t;
    mGoalText->SetText( mText );
    mCurrentY = g_resH * 0.45f;
}

Goal::Goal() {
    mWaitTimer = ruTimer::Create();
    mInitialY = 20;
    mCurrentY = mInitialY;
    mDestY = 20;
    mWaitSec = 2.0f;
    mAlpha = 255.0f;
    mGoalText = ruText::Create( "Goal", 40, mCurrentY, g_resW - 80, 32, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center, mAlpha );
}

Goal::~Goal() {
	mGoalText->Free();
	mWaitTimer->Free();
}

void Goal::SetVisible( bool state ) {
    mGoalText->SetVisible( state );
}
