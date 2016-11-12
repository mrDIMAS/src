#include "Precompiled.h"
#include "LevelEnding.h"

LevelEnding::LevelEnding(const unique_ptr<PlayerTransfer>& playerTransfer) :
	Level(playerTransfer)
{
	CreateBlankScene();

	mCamera = ruCamera::Create(60);

	mName = LevelName::Ending;
	LoadLocalization("ending.loc");
	mMusic = ruSound::LoadMusic("data/music/ending.ogg");

	mGUIScene = ruGUIScene::Create();

	mLargeFont = ruFont::LoadFromFile(22, "data/fonts/font5.ttf");
	mBackground = mGUIScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request("data/textures/generic/black.jpg"), ruVector3(0, 0, 0));
	mEndingText = mGUIScene->CreateText(mLocalization.GetString("text"), 0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, mLargeFont, ruVector3(255, 255, 255), ruTextAlignment::Center);
	mEndingText->SetPosition(0, mEndingText->GetSize().y * 1.5);	


}

LevelEnding::~LevelEnding() {

}

void LevelEnding::DoScenario() {
	mEndingText->Move(ruVector2(0, -0.9));
	mMusic->Play();
}

void LevelEnding::OnSerialize(SaveFile & out) {

}
