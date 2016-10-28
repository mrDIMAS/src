#include "Precompiled.h"

#include "LevelIntroduction.h"

void LevelIntroduction::Hide() {

}

void LevelIntroduction::Show() {

}

void LevelIntroduction::DoScenario() {
	if (mShowIntro) {
		mTextAlphaTo = 255.0f;
	}
	else {
		mTextAlphaTo = 0.0f;
	}

	float fadeSpeed = 0.025f;

	if (!mShowIntro) {
		fadeSpeed = 0.05f;
	}

	mTextAlpha += (mTextAlphaTo - mTextAlpha) * 0.025f;

	if (ruInput::IsKeyHit(ruInput::Key::Space)) {
		mShowIntro = false;
	}

	mGUIText->SetAlpha(mTextAlpha);
	mGUISkipText->SetVisible(mShowIntro);

	if (mShowIntro == false) {
		if (mTextAlpha < 5.0f) {
			Level::Change(LevelName::Arrival);
		}
	}

	DoneInitialization();
}

LevelIntroduction::~LevelIntroduction() {

}

LevelIntroduction::LevelIntroduction(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer) {
	mGUIScene = ruGUIScene::Create();
	CreateBlankScene();
	mTextAlpha = 0.0f;
	mTextAlphaTo = 255.0f;
	mShowIntro = true;
	mName = LevelName::Introduction;
	LoadLocalization("intro.loc");
	int scx = ruVirtualScreenWidth / 2;
	int scy = ruVirtualScreenHeight / 2;
	int w = 600;
	int h = 400;
	mGUIBackground = mGUIScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request("data/textures/generic/loadingScreen.jpg"));
	mGUIText = mGUIScene->CreateText(mLocalization.GetString("intro"), scx - w / 2, scy - h / 2, w, h, pGUIProp->mFont, ruVector3(0, 0, 0), ruTextAlignment::Left, mTextAlpha);
	mGUISkipText = mGUIScene->CreateText(mLocalization.GetString("skip"), ruVirtualScreenWidth / 2 - 256, ruVirtualScreenHeight - 200, 512, 128, pGUIProp->mFont, ruVector3(255, 0, 0), ruTextAlignment::Center);
	
}

void LevelIntroduction::OnDeserialize(SaveFile & in) {

}

void LevelIntroduction::OnSerialize(SaveFile & out) {

}
