#include "Precompiled.h"
#include "LevelForest.h"

LevelForest::LevelForest(unique_ptr<Game> & game, const unique_ptr<PlayerTransfer>& playerTransfer) :
	Level(game, playerTransfer) {
	mName = LevelName::Forest;

	LoadLocalization("forest.loc");
	LoadSceneFromFile("data/maps/forest.scene");
	mPlayer->SetPosition(GetUniqueObject("PlayerPosition")->GetPosition());
	mPlayer->GetHUD()->SetObjective(mLocalization.GetString("objectiveToHighway"));

	mMusic = game->GetEngine()->GetSoundSystem()->LoadMusic("data/music/forest.ogg");

	auto renderer = mGame->GetEngine()->GetRenderer();
	mPlayer->mpCamera->mCamera->SetSkybox(
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonUp2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonRight2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonLeft2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonFront2048.png"),
		renderer->GetTexture("data/textures/skyboxes/FullMoon/FullMoonBack2048.png")
	);

	AddSound(mWindSound = game->GetEngine()->GetSoundSystem()->LoadSound2D("data/sounds/wind.ogg"));
	mWindSound->SetVolume(0.5f);
	mWindSound->Play();

	mZoneEnd = GetUniqueObject("GameEndZone");

	//IRenderer::LoadColorGradingMap("data/textures/colormaps/greyblueshade.png");

	mWater = GetUniqueObject("Water");

	auto fogMesh = GetUniqueObject("Fog");
	mFog = mGame->GetEngine()->GetSceneFactory()->CreateFog(fogMesh->GetAABBMin(), fogMesh->GetAABBMax(), Vector3(0.5, 0.5, 0.7), 0.2);
	mFog->SetPosition(fogMesh->GetPosition());
	mFog->SetSpeed(Vector3(0.0005, 0, 0.0005));
	mFog->Attach(mScene);

	DoneInitialization();
}

LevelForest::~LevelForest() {

}

void LevelForest::DoScenario() {
	mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0.1, 0.1, 0.1));

	mWater->SetTexCoordFlow(Vector2(0.0, -mWaterFlow));
	mWaterFlow += 0.00025f;

	mMusic->SetVolume(0.65);
	mMusic->Play();

	if(mPlayer->IsInsideZone(mZoneEnd)) {
		mGame->LoadLevel(LevelName::Ending);
	}
}

void LevelForest::OnSerialize(SaveFile & out) {

}
