#include "Precompiled.h"

#include "WaitKeyButton.h"
#include "Utils.h"

vector<WaitKeyButton*> WaitKeyButton::msWaitKeyList;

void WaitKeyButton::Update() {
	if(mGUIButton->IsHit()) {
		// skip "grab key" mode from other buttons
		for(auto pWaitKey : WaitKeyButton::msWaitKeyList) {
			pWaitKey->mGrabKey = false;
		}
		mGrabKey = true;
	}

	if(mGrabKey) {
		for(int i = static_cast<int>(ruInput::Key::Num1); i < static_cast<int>(ruInput::Key::Count); i++) {
			if(mGame->GetEngine()->GetInput()->IsKeyDown(static_cast<ruInput::Key>(i))) {
				SetSelected(static_cast<ruInput::Key>(i));
				mGrabKey = false;
			}
		}
		if(mGame->GetEngine()->GetInput()->IsKeyHit(ruInput::Key::Esc)) {
			mGrabKey = false;
		}

		if(mAnimCounter < 10) {
			mGUIButton->GetText()->SetText("[ Key ]");
		} else {
			mGUIButton->GetText()->SetText("[Key]");
		}
		if(mAnimCounter > 20) {
			mAnimCounter = 0;
		}
		mAnimCounter++;
	} else {
		mGUIButton->GetText()->SetText(mGame->GetEngine()->GetInput()->GetKeyName(mSelectedKey));
	}
}

void WaitKeyButton::SetSelected(ruInput::Key i) {
	mDesc = mGame->GetEngine()->GetInput()->GetKeyName(i);
	ruInput::Key lastKey = mSelectedKey;
	mSelectedKey = i;
	// swap keys if duplicate found
	for(auto pWaitKey : WaitKeyButton::msWaitKeyList) {
		if(pWaitKey != this) {
			if(pWaitKey->mSelectedKey == mSelectedKey) {
				pWaitKey->mSelectedKey = lastKey;
			}
		}
	}
}

WaitKeyButton::WaitKeyButton(unique_ptr<Game> & game, const shared_ptr<ruGUIScene> & scene, float x, float y, shared_ptr<ruTexture> buttonImage, const string & text) :
	mGame(game) {
	int textHeight = 16;
	float buttonWidth = 60;
	float buttonHeight = 32;
	mDesc = " ";
	mAnimCounter = 0;
	mGrabKey = false;
	mGUIButton = scene->CreateButton(x, y, buttonWidth, buttonHeight, buttonImage, "[Key]", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
	mGUIText = scene->CreateText(text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
	WaitKeyButton::msWaitKeyList.push_back(this);
}

ruInput::Key WaitKeyButton::GetSelectedKey() {
	return mSelectedKey;
}

void WaitKeyButton::SetVisible(bool state) {
	mGUIText->SetVisible(state);
	mGUIButton->SetVisible(state);
}

void WaitKeyButton::AttachTo(const shared_ptr<ruGUINode> & node) {
	mGUIText->Attach(node);
	mGUIButton->Attach(node);
}

WaitKeyButton::~WaitKeyButton() {
	auto existing = find(msWaitKeyList.begin(), msWaitKeyList.end(), this);
	if(existing != msWaitKeyList.end()) {
		msWaitKeyList.erase(existing);
	}
}

