#include "Precompiled.h"

#include "Goal.h"

void Goal::AnimateAndRender()
{
	if(mWaitTimer->GetElapsedTimeInSeconds() >= mWaitSec) {
		mCurrentY += (mDestY - mCurrentY) * 0.05f;
	}

	mAlpha = mAlpha + (100 - mAlpha) * 0.05f;

	// pulse 
	if(mAlpha <= 101) {
		mAlpha = 255.0f;
	}

	mGoalText->SetAlpha(mAlpha);
	mGoalText->SetPosition(40, mCurrentY);
}

void Goal::SetText(string t)
{
	mWaitTimer->Restart();
	mDestY = mInitialY;
	mText = t;
	mGoalText->SetText(mText);
	mCurrentY = ruVirtualScreenHeight * 0.45f;
}

Goal::Goal(const shared_ptr<IGUIScene> & scene)
{
	mWaitTimer = ITimer::Create();
	mInitialY = 20;
	mCurrentY = mInitialY;
	mDestY = 20;
	mWaitSec = 2.0f;
	mAlpha = 255.0f;
	mGoalText = scene->CreateText("Goal", 40, mCurrentY, ruVirtualScreenWidth - 80, 32, pGUIProp->mFont, pGUIProp->mNoticeColor, TextAlignment::Center, mAlpha);
}

Goal::~Goal()
{

}

void Goal::SetVisible(bool state)
{
	mGoalText->SetVisible(state);
}
