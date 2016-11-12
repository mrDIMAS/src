#include "Precompiled.h"
#include "LevelCutsceneIntro.h"

LevelIntro::LevelIntro(const unique_ptr<PlayerTransfer> & playerTransfer) : Level(playerTransfer), mEnginePitch(1.0f, 0.65f, 1.25f) {
	mName = LevelName::Intro;

	LoadSceneFromFile("data/maps/intro.scene");
	mUAZ = GetUniqueObject("UAZ");
	mLastUAZPosition = mUAZ->GetPosition();

	mUAZAnim = ruAnimation(0, 350, 20);
	mUAZ->SetAnimation(&mUAZAnim);
	mUAZAnim.SetEnabled(true);

	shared_ptr<ruSceneNode> deerBone = GetUniqueObject("Bone012");
	mDeerAnim = ruAnimation(0, 350, 35);
	deerBone->SetAnimation(&mDeerAnim);
	mDeerAnim.SetEnabled(true);

	AddSound(mEngineLoop = ruSound::Load3D("data/sounds/engineloop.ogg"));
	mEngineLoop->Attach(GetUniqueObject("Engine"));
	mEngineLoop->SetLoop(true);
	mEngineLoop->Play();

	mNewLevelLoadZone = GetUniqueObject("NewLevelLoadZone");

	mChangeCameraZone1 = GetUniqueObject("ChangeCamera1");
	mChangeCameraZone2 = GetUniqueObject("ChangeCamera2");
	mChangeCameraZone3 = GetUniqueObject("ChangeCamera3");

	mCameraPivot = GetUniqueObject("Camera");
	mCameraAnim1 = ruAnimation(0, 120, 12);
	mCameraPivot->SetAnimation(&mCameraAnim1);
	mCameraAnim1.SetEnabled(true);

	mCameraPivot2 = GetUniqueObject("Camera2");
	mCameraPivot3 = GetUniqueObject("Camera3");
	mCameraPivot4 = GetUniqueObject("Camera4");

	mCamera = new GameCamera(nullptr);
	mCamera->mCamera->Attach(mCameraPivot);
	mCamera->mCamera->SetSkybox(
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonUp2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonRight2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonLeft2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonFront2048.png"),
		ruTexture::Request("data/textures/skyboxes/FullMoon/FullMoonBack2048.png")
	);


	// text
	mTextAlpha = 0.0f;
	mTextAlphaTo = 255.0f;
	mShowIntro = true;
	LoadLocalization("intro.loc");
	int scx = ruVirtualScreenWidth / 2;
	int scy = ruVirtualScreenHeight / 2;
	int w = 600;
	int h = 400;
	mGUIScene = ruGUIScene::Create();
	mGUIText = mGUIScene->CreateText(mLocalization.GetString("intro"), scx - w / 2, scy - h / 2, w, h, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left, mTextAlpha);
	mGUISkipText = mGUIScene->CreateText(mLocalization.GetString("skip"), ruVirtualScreenWidth / 2 - 256, ruVirtualScreenHeight - 200, 512, 128, pGUIProp->mFont, ruVector3(255, 0, 0), ruTextAlignment::Center);

	DoneInitialization();
}

LevelIntro::~LevelIntro() {

}

void LevelIntro::DoScenario() {
	// force disable hdr
	//ruEngine::SetHDREnabled(false);

	mCamera->MakeCurrent();
	mUAZAnim.Update();
	mDeerAnim.Update();
	mCameraAnim1.Update();
	ruEngine::SetAmbientColor(ruVector3(0.095, 0.095, 0.15));

	if (mUAZ->IsInsideNode(mChangeCameraZone1)) {
		mCamera->mCamera->Attach(mCameraPivot2);
	}

	if (mUAZ->IsInsideNode(mChangeCameraZone2)) {
		mCamera->mCamera->Attach(mCameraPivot3);
	}

	if (mUAZ->IsInsideNode(mChangeCameraZone3)) {
		mCamera->mCamera->Attach(mCameraPivot4);
	}

	mEnginePitch.SetTarget((mUAZ->GetPosition() - mLastUAZPosition).Length() * 10);
	mEnginePitch.ChaseTarget(0.1f);

	mLastUAZPosition = mUAZ->GetPosition();
	mEngineLoop->SetPitch(mEnginePitch);

	// text
	if (mShowIntro) {
		mTextAlphaTo = 255.0f;
	} else {
		mTextAlphaTo = 0.0f;
	}

	mTextAlpha += (mTextAlphaTo - mTextAlpha) * 0.075f;

	if (ruInput::IsKeyHit(ruInput::Key::Space)) {
		mShowIntro = false;
	}

	mGUIText->SetAlpha(mTextAlpha);
	mGUISkipText->SetVisible(mShowIntro);

	if (mUAZ->IsInsideNode(mNewLevelLoadZone) || (mShowIntro == false && mTextAlpha < 5.0f)) {
		Level::Change(LevelName::Arrival);
	}
}

void LevelIntro::OnDeserialize(SaveFile & in) {

}

void LevelIntro::OnSerialize(SaveFile & out) {

}

