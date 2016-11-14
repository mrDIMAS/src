#include "Precompiled.h"
#include "LevelForest.h"

LevelForest::LevelForest(const unique_ptr<PlayerTransfer>& playerTransfer) : Level(playerTransfer) {
	mName = LevelName::Forest;

	LoadLocalization("forest.loc");
	LoadSceneFromFile("data/maps/forest.scene");
	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());
	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveToHighway"));

	mMusic = ruSound::LoadMusic("data/music/forest.ogg");

	mPlayer->mpCamera->mCamera->SetSkybox(
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonUp2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonRight2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonLeft2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonFront2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonBack2048.png")
	);

	AddSound(mWindSound = ruSound::Load2D("data/sounds/wind.ogg"));
	mWindSound->SetVolume(0.5f);
	mWindSound->Play();

	mZoneEnd = GetUniqueObject("GameEndZone");

	//ruEngine::LoadColorGradingMap("data/textures/colormaps/greyblueshade.png");

	mWater = GetUniqueObject("Water");

	auto fogMesh = GetUniqueObject("Fog");
	mFog = ruFog::Create(fogMesh->GetAABBMin(), fogMesh->GetAABBMax(), ruVector3(0.5, 0.5, 0.7), 0.15);
	mFog->SetPosition(fogMesh->GetPosition());
	mFog->SetSpeed(ruVector3(0.0015, 0, 0.0015));
	mFog->Attach(mScene);

	DoneInitialization();
}

LevelForest::~LevelForest() {

}

void LevelForest::DoScenario() {
	ruEngine::SetAmbientColor(ruVector3(0.1, 0.1, 0.1));

	mWater->SetTexCoordFlow(ruVector2(0.0, -mWaterFlow));
	mWaterFlow += 0.00025f;

	mMusic->SetVolume(0.65);
	mMusic->Play();

	if (mPlayer->IsInsideZone(mZoneEnd)) {
		Level::Change(LevelName::Ending);
	}
}

void LevelForest::OnSerialize(SaveFile & out) {

}
