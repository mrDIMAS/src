#include "Precompiled.h"
#include "LevelForest.h"

LevelForest::LevelForest(const unique_ptr<PlayerTransfer>& playerTransfer) : Level(playerTransfer) {
	mName = LevelName::Forest;

	LoadLocalization("forest.loc");

	LoadSceneFromFile("data/maps/forest.scene");

	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());

	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveToHighway"));


	mPlayer->mpCamera->mCamera->SetSkybox(
		ruTexture::Request("data/textures/skyboxes/DarkStormy/DarkStormyUp2048.png"),
		ruTexture::Request("data/textures/skyboxes/DarkStormy/DarkStormyRight2048.png"),
		ruTexture::Request("data/textures/skyboxes/DarkStormy/DarkStormyLeft2048.png"),
		ruTexture::Request("data/textures/skyboxes/DarkStormy/DarkStormyFront2048.png"),
		ruTexture::Request("data/textures/skyboxes/DarkStormy/DarkStormyBack2048.png")
	);

	AddSound(mWindSound = ruSound::Load2D("data/sounds/wind.ogg"));
	mWindSound->SetVolume(0.5f);
	mWindSound->Play();

	mZoneEnd = GetUniqueObject("GameEndZone");

	ruEngine::LoadColorGradingMap("data/textures/colormaps/greyblueshade.png");

	DoneInitialization();
}

LevelForest::~LevelForest() {

}

void LevelForest::DoScenario() {
	ruEngine::SetAmbientColor(ruVector3(0.3, 0.3, 0.3));

	if (mPlayer->IsInsideZone(mZoneEnd)) {
		Level::Change(LevelName::Ending);
	}
}

void LevelForest::OnSerialize(SaveFile & out) {

}
