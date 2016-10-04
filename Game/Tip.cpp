#include "Precompiled.h"

#include "Tip.h"

void Tip::AnimateAndDraw() {
	mGUIText->SetPosition(mX, mY);
	mGUIText->SetAlpha(mAlpha);
	if (mTimer->GetElapsedTimeInSeconds() > 1.5) {
		mAlpha.SetTarget(mAlpha.GetMin());
		mAlpha.ChaseTarget(0.1);
		mY.SetTarget(mY.GetMax());
		mY.ChaseTarget(0.1);
	};
}

void Tip::SetNewText(string text) {
	mTimer->Restart();
	mAlpha.Set(255.0f);
	mGUIText->SetText(text);
	mY.SetMax(ruVirtualScreenHeight - mHeight);
	mY.SetMin(ruVirtualScreenHeight / 2 + mHeight);
	mY.Set(mY.GetMin());
}

Tip::Tip(const shared_ptr<ruGUIScene> & scene) : mAlpha(255.0f, 0.0f, 255.0f) {
	mTimer = ruTimer::Create();
	mWidth = 256;
	mHeight = 32;
	mX = ruVirtualScreenWidth / 2 - mWidth / 2;
	mGUIText = scene->CreateText(" ", 0, 0, mWidth, mHeight, pGUIProp->mFont, pGUIProp->mNoticeColor, ruTextAlignment::Center, mAlpha);
	SetNewText(" ");
}

void Tip::SetVisible(bool state) {
	mGUIText->SetVisible(state);
}

Tip::~Tip() {

}
