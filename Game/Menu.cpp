#include "Precompiled.h"
#include "Menu.h"
#include "Level.h"
#include "GUIProperties.h"
#include "Player.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"

unique_ptr<Menu> pMainMenu;

string GetFileCreationDate(const string & pFileName) {
	stringstream ss;
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	SYSTEMTIME st;
	SYSTEMTIME localTime;

	GetFileAttributesExA(pFileName.c_str(), GetFileExInfoStandard, &wfad);
	FileTimeToSystemTime(&wfad.ftLastWriteTime, &st);

	SystemTimeToTzSpecificLocalTime(nullptr, &st, &localTime);

	ss << setfill('0') << setw(2) << localTime.wDay << '/' <<
		setfill('0') << setw(2) << localTime.wMonth << '/' <<
		setw(4) << localTime.wYear << " - " <<
		setfill('0') << setw(2) << localTime.wHour << ":" <<
		setfill('0') << setw(2) << localTime.wMinute << ":" <<
		setfill('0') << setw(2) << localTime.wSecond;

	return ss.str();
}

Menu::Menu() :
	mDistBetweenButtons(68),
	mVisible(true),
	mPage(Page::Main),
	mLoadSaveGameName("") {
	mLocalization.ParseFile(gLocalizationPath + "menu.loc");
	// load background scene
	mScene = ruSceneNode::LoadFromFile("data/maps/menu.scene");
	// create gui scene
	mGUIScene = ruGUIScene::Create();
	// create camera
	mpCamera = unique_ptr<GameCamera>(new GameCamera(mGUIScene));
	mCameraFadeActionDone = false;
	mCameraInitialPosition = mScene->FindChild("Camera")->GetPosition();
	mCameraAnimationNewOffset = ruVector3(0.5, 0.5, 0.5);

	mPickSound = ruSound::Load2D("data/sounds/menupick.ogg");
	mMusic = ruSound::LoadMusic("data/music/menu.ogg");

	const float buttonHeight = 30;
	const float buttonWidth = 128;
	const float buttonXOffset = 10;

	// load textures
	auto texTab = ruTexture::Request("data/gui/menu/tab.tga");
	auto texButton = ruTexture::Request("data/gui/menu/button.tga");
	auto texSmallButton = ruTexture::Request("data/gui/menu/button.tga");

	// Setup
	mCanvas = mGUIScene->CreateRect(0, 0, 0, 0, nullptr);
	{
		// Main 
		mGUIMainButtonsCanvas = mGUIScene->CreateRect(20, ruVirtualScreenHeight - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8.5, texTab, pGUIProp->mBackColor);
		mGUIMainButtonsCanvas->Attach(mCanvas);
		{
			mContinueGameButton = mGUIScene->CreateButton(buttonXOffset, 5, buttonWidth, buttonHeight, texButton, mLocalization.GetString("continueButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mContinueGameButton->Attach(mGUIMainButtonsCanvas);
			mContinueGameButton->AddAction(ruGUIAction::OnClick, [this] { OnContinueGameClick(); });

			mStartButton = mGUIScene->CreateButton(buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("startButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mStartButton->Attach(mGUIMainButtonsCanvas);
			mStartButton->AddAction(ruGUIAction::OnClick, [this] { OnStartNewGameClick(); });

			mSaveGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("saveButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mSaveGameButton->Attach(mGUIMainButtonsCanvas);
			mSaveGameButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::SaveGame); FillListOfSaveFiles(); });

			mLoadGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("loadButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mLoadGameButton->Attach(mGUIMainButtonsCanvas);
			mLoadGameButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::LoadGame); FillListOfSaveFiles(); });

			mOptionsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("optionsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mOptionsButton->Attach(mGUIMainButtonsCanvas);
			mOptionsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::Options); });

			mAuthorsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("authorsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mAuthorsButton->Attach(mGUIMainButtonsCanvas);
			mAuthorsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::Authors); });

			mExitButton = mGUIScene->CreateButton(buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("exitButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mExitButton->Attach(mGUIMainButtonsCanvas);
			mExitButton->AddAction(ruGUIAction::OnClick, [this] { OnExitGameClick(); });
		}

		const int aTabX = 200;
		const int aTabY = ruVirtualScreenHeight - 4.0 * mDistBetweenButtons;
		const int aTabWidth = buttonWidth * 4;
		const int aTabHeight = buttonHeight * 8.5;

		// Modal window
		mModalWindow = unique_ptr<ModalWindow>(new ModalWindow(mGUIScene, aTabX, aTabY, aTabWidth, aTabHeight, texTab, texButton, pGUIProp->mBackColor));
		mModalWindow->AttachTo(mCanvas);

		// Page title
		mWindowText = mGUIScene->CreateText(" ", aTabX, aTabY - 21, aTabWidth, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mWindowText->Attach(mCanvas);

		// Product name
		mCaption = mGUIScene->CreateText("The Mine", 20, aTabY - 21, aTabWidth * 1.5f, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mCaption->Attach(mCanvas);

		// Options
		mGUIOptionsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mCanvas);
		{
			const int yOffset = (aTabHeight - 2 * mDistBetweenButtons) / 2;

			mGUIOptionsCommonButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset, buttonWidth, buttonHeight, texButton, mLocalization.GetString("commonSettings"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsCommonButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsCommonButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::OptionsCommon); });

			mGUIOptionsControlsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("controls"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsControlsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsControlsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::OptionsKeys); });

			mGUIOptionsGraphicsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("graphics"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsGraphicsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsGraphicsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::OptionsGraphics); });
		}

		// Options: Keys
		mOptionsKeysCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mCanvas);
		mOptionsKeysCanvas->SetVisible(false);
		{
			// First column
			float x = 40, y = 10;

			mMoveForwardKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("forward"));
			mMoveForwardKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mMoveBackwardKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("backward"));
			mMoveBackwardKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStrafeLeftKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeLeft"));
			mStrafeLeftKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStrafeRightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeRight"));
			mStrafeRightKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mJumpKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("jump"));
			mJumpKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mFlashLightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("flashLight"));
			mFlashLightKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mRunKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("run"));
			mRunKey->AttachTo(mOptionsKeysCanvas);

			// Second column
			x += 150;
			y = 10;
			mInventoryKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("inventory"));
			mInventoryKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mUseKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("use"));
			mUseKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mQuickLoadKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickLoad"));
			mQuickLoadKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mQuickSaveKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickSave"));
			mQuickSaveKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStealthKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("stealth"));
			mStealthKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mLookLeftKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookLeft"));
			mLookLeftKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mLookRightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookRight"));
			mLookRightKey->AttachTo(mOptionsKeysCanvas);
		}

		// Options: Graphics
		mOptionsGraphicsCanvas = mGUIScene->CreateRect(aTabX, aTabY, buttonWidth * 5.5, aTabHeight, texTab, pGUIProp->mBackColor);
		mOptionsGraphicsCanvas->Attach(mCanvas);
		mOptionsGraphicsCanvas->SetVisible(false);
		{
			float x = 30, y = 10;

			mFXAAButton = make_unique<RadioButton>(mGUIScene, x, y, texButton, mLocalization.GetString("fxaa"));
			mFXAAButton->AttachTo(mOptionsGraphicsCanvas);

			mFPSButton = make_unique<RadioButton>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("showFPS"));
			mFPSButton->AttachTo(mOptionsGraphicsCanvas);

			mSpotShadowsButton = make_unique<RadioButton>(mGUIScene, x, y + mDistBetweenButtons, texButton, mLocalization.GetString("spotLightShadows"));
			mSpotShadowsButton->AttachTo(mOptionsGraphicsCanvas);

			mPointShadowsButton = make_unique<RadioButton>(mGUIScene, x, y + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("pointLightShadows"));
			mPointShadowsButton->AttachTo(mOptionsGraphicsCanvas);

			mHDRButton = make_unique<RadioButton>(mGUIScene, x, y + 2.0 * mDistBetweenButtons, texButton, mLocalization.GetString("hdr"));
			mHDRButton->AttachTo(mOptionsGraphicsCanvas);

			mParallaxButton = make_unique<RadioButton>(mGUIScene, x, y + 2.5 * mDistBetweenButtons, texButton, mLocalization.GetString("parallax"));
			mParallaxButton->AttachTo(mOptionsGraphicsCanvas);

			mVolumetricFogButton = make_unique<RadioButton>(mGUIScene, x, y + 3.0 * mDistBetweenButtons, texButton, mLocalization.GetString("volumetricFog"));
			mVolumetricFogButton->AttachTo(mOptionsGraphicsCanvas);

			// next column
			x += 170;
			mSoftShadows = make_unique<RadioButton>(mGUIScene, x, y, texButton, mLocalization.GetString("softShadows"));
			mSoftShadows->AttachTo(mOptionsGraphicsCanvas);

			mDynamicDirectionalLightShadows = make_unique<RadioButton>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("dynamicDirectionalLightShadows"));
			mDynamicDirectionalLightShadows->AttachTo(mOptionsGraphicsCanvas);

			mBloom = make_unique<RadioButton>(mGUIScene, x, y + 1.0 * mDistBetweenButtons, texButton, mLocalization.GetString("bloom"));
			mBloom->AttachTo(mOptionsGraphicsCanvas);

			// next column
			x += 170;

			mResolutionList = make_unique<ScrollList>(mGUIScene, x, y, texButton, mLocalization.GetString("resolution"));
			mResolutionList->AttachTo(mOptionsGraphicsCanvas);
			auto videoModes = ruEngine::GetVideoModeList();
			for (auto videoMode : videoModes) {
				mResolutionList->AddValue(StringBuilder() << videoMode.mWidth << "x" << videoMode.mHeight << "@" << videoMode.mRefreshRate);
			}
			mResolutionList->OnChange += [this] { mSettingsApplied->SetText(mLocalization.GetString("settingsNotApplied")); mSettingsApplied->SetColor(ruVector3(255, 0, 0)); };

			mWindowMode = make_unique<ScrollList>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("windowMode"));
			mWindowMode->AttachTo(mOptionsGraphicsCanvas);
			mWindowMode->AddValue(mLocalization.GetString("fullscreen"));
			mWindowMode->AddValue(mLocalization.GetString("windowed"));
			mWindowMode->AddValue(mLocalization.GetString("borderless"));
			mWindowMode->OnChange += [this] { mSettingsApplied->SetText(mLocalization.GetString("settingsNotApplied")); mSettingsApplied->SetColor(ruVector3(255, 0, 0)); };

			mTextureFiltering = make_unique<ScrollList>(mGUIScene, x, y + 1.0 * mDistBetweenButtons, texButton, mLocalization.GetString("filtering"));
			mTextureFiltering->AttachTo(mOptionsGraphicsCanvas);
			mTextureFiltering->AddValue(mLocalization.GetString("trilinear"));
			for (int i = 1; i <= ruEngine::GetMaxIsotropyDegree(); ++i) {
				mTextureFiltering->AddValue(StringBuilder() << mLocalization.GetString("anisotropic") << " x" << i);
			}

			mSpotLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("spotLightShadowMap"));
			mSpotLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for (int i = 256; i <= 2048; i *= 2) {
				mSpotLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mPointLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 2.0 * mDistBetweenButtons, texButton, mLocalization.GetString("pointLightShadowMap"));
			mPointLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for (int i = 128; i <= 1024; i *= 2) {
				mPointLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mDirectionalLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 2.5 * mDistBetweenButtons, texButton, mLocalization.GetString("directionalLightShadowMap"));
			mDirectionalLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for (int i = 1024; i <= 4096; i *= 2) {
				mDirectionalLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mSettingsApplied = mGUIScene->CreateText(mLocalization.GetString("settingsApplied"), x, y + 3.1 * mDistBetweenButtons, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3(0, 255, 0), ruTextAlignment::Left);
			mSettingsApplied->Attach(mOptionsGraphicsCanvas);
		}

		// Authors
		mAuthorsBackground = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mAuthorsBackground->Attach(mCanvas);
		{
			mGUIAuthorsText = mGUIScene->CreateText(mLocalization.GetString("authorsText"), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
			mGUIAuthorsText->Attach(mAuthorsBackground);
		}

		// Save Game
		mSaveGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mSaveGameCanvas->Attach(mCanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mSaveGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mSaveGameSlot[i]->Attach(mSaveGameCanvas);
				mSaveGameSlot[i]->AddAction(ruGUIAction::OnClick, [this] { OnCreateSaveClick(); });

				mSaveGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
				mSaveGameFileTime[i]->Attach(mSaveGameCanvas);

				y += 1.1f * buttonHeight;
			}
		}

		// Load Game
		mLoadGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mLoadGameCanvas->Attach(mCanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mLoadGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mLoadGameSlot[i]->Attach(mLoadGameCanvas);
				mLoadGameSlot[i]->AddAction(ruGUIAction::OnClick, [this] { OnLoadSaveClick(); });

				mLoadGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
				mLoadGameFileTime[i]->Attach(mLoadGameCanvas);
				y += 1.1f * buttonHeight;
			}
		}

		// Options: Common
		mOptionsCommonCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mOptionsCommonCanvas->Attach(mCanvas);
		mOptionsCommonCanvas->SetVisible(false);
		{
			const int yOffset = (aTabHeight - 1.5 * mDistBetweenButtons) / 2;
			const int xOffset = aTabWidth / 6.5;

			mFOVSlider = make_unique<Slider>(mGUIScene, xOffset, yOffset - 0.5f * mDistBetweenButtons, 55, 90, 1.0f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("fov"));
			mFOVSlider->AttachTo(mOptionsCommonCanvas);

			mMasterVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("masterVolume"));
			mMasterVolume->AttachTo(mOptionsCommonCanvas);
			mMasterVolume->SetChangeAction([this] { ruSound::SetMasterVolume(mMasterVolume->GetValue() / 100.0f); });

			mMusicVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("musicVolume"));
			mMusicVolume->AttachTo(mOptionsCommonCanvas);
			mMusicVolume->SetChangeAction([this] { OnMusicVolumeChange(); });

			mMouseSensivity = make_unique<Slider>(mGUIScene, xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("mouseSens"));
			mMouseSensivity->AttachTo(mOptionsCommonCanvas);
			mMouseSensivity->SetChangeAction([this] { gMouseSens = mMouseSensivity->GetValue() / 100.0f; });
		}
	}

	SetAuthorsPageVisible(false);
	SetPage(Page::Main);
	LoadConfig();
}

float Menu::GetMusicVolume() {
	return mMusicVolume->GetValue() / 100.0f;
}

void Menu::Show() {
	mpCamera->MakeCurrent();
	mpCamera->FadeIn();
	mScene->Show();
	mCaption->SetVisible(true);
	if (Level::Current()) {
		Level::Current()->Hide();
		auto & player = Level::Current()->GetPlayer();
		if (player) {
			player->SetBodyVisible(false);
			player->SetHUDVisible(false);
		}
	}
	mVisible = true;
	ruEngine::ShowCursor();
	mGUIScene->SetVisible(true);
	mGUIScene->SetOpacity(1.0f);
}

void Menu::Hide() {
	mScene->Hide();
	mCaption->SetVisible(false);
	mWindowText->SetVisible(false);
	if (Level::Current()) {
		Level::Current()->Show();
		auto & player = Level::Current()->GetPlayer();
		if (player) {
			player->mpCamera->MakeCurrent();
			player->SetBodyVisible(true);
			player->SetHUDVisible(true);
		}
	}
	mVisible = false;
	mPage = Page::Main;
	ruEngine::HideCursor();
	mMusic->Pause();
	mGUIScene->SetVisible(false);
}

void Menu::OnStartNewGameClick() {
	SetPage(Page::Main);
	mModalWindow->Ask(mLocalization.GetString("newGameQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { Level::Change(g_initialLevel);	Hide(); }); });
}

void Menu::OnExitGameClick() {
	SetPage(Page::Main);
	mModalWindow->Ask(mLocalization.GetString("endGameQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { DoExitGame(); }); });
}

void Menu::OnContinueGameClick() {
	SetPage(Page::Main);
	if (!Level::Current()) {
		mModalWindow->Ask(mLocalization.GetString("continueLastGameQuestion"));
		mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader("quickSave.save").RestoreWorldState(); Hide(); }); });
	} else {
		CameraStartFadeOut([this] {	Hide(); });
	}
}

void Menu::DoExitGame() {
	if (Level::Current()) {
		auto & player = Level::Current()->GetPlayer();
		if (player) {
			if (!player->mDead) {
				SaveWriter("lastGame.save").SaveWorldState();
			}
		}
	}
	gRunning = false;
	WriteConfig();
}

void Menu::UpdateCamera() {
	mpCamera->Update();
	if (!mCameraFadeActionDone) {
		mGUIScene->SetOpacity(mGUIScene->GetOpacity() - 0.05f); // disappear smoothly
	}
	if (mpCamera->FadeComplete()) {
		if (!mCameraFadeActionDone) {
			if (mCameraFadeDoneAction) {
				mCameraFadeDoneAction();
			}
			mCameraFadeActionDone = true;
		}
	}
}

void Menu::OnLoadSaveClick() {
	SetPage(Page::Main, false);
	for (int i = 0; i < mSaveLoadSlotCount; ++i) {
		if (mLoadGameSlot[i]->IsHit()) {
			mLoadSaveGameName = mSaveFilesList[i];
		}
	}
	mModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mLoadSaveGameName << "." << mLocalization.GetString("loadSaveQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader(mLoadSaveGameName).RestoreWorldState();	Hide(); }); });
}

void Menu::OnCreateSaveClick() {
	SetPage(Page::Main, false);
	for (int i = 0; i < mSaveLoadSlotCount; ++i) {
		if (mSaveGameSlot[i]->IsHit()) {
			mSaveGameSlotName = mSaveFilesList[i];
		}
	}
	mModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mSaveGameSlotName << "." << mLocalization.GetString("rewriteSaveQuestion"));
	mModalWindow->SetYesAction([this] { SaveWriter(mSaveGameSlotName).SaveWorldState(); SetPage(Page::Main); });
}

void Menu::OnMusicVolumeChange() {
	mMusic->SetVolume(mMusicVolume->GetValue() / 100.0f);
	gMusicVolume = mMusicVolume->GetValue() / 100.0f;
	if (Level::Current()) {
		Level::Current()->mMusic->SetVolume(gMusicVolume);
	}
}

void Menu::CameraStartFadeOut(const ruDelegate & onFadeDoneAction) {
	mpCamera->FadeOut();
	mCameraFadeActionDone = false;
	mCameraFadeDoneAction = onFadeDoneAction;
}

void Menu::Update() {
	WaitKeyButton::UpdateAll();
	UpdateCamera();
	SyncPlayerControls();

	if (mVisible) {
		AnimateCamera();
		ruEngine::SetAmbientColor(ruVector3(0.1, 0.1, 0.1));

		if (Level::Current()) {
			// back to menu by pressing [Esc]
			if (ruInput::IsKeyHit(ruInput::Key::Esc)) {
				CameraStartFadeOut([this] {	Hide(); });
			}

			// switch on/off buttons if player dead or alive
			auto & player = Level::Current()->GetPlayer();
			if (player) {
				if (player->IsDead()) {
					mContinueGameButton->SetActive(false);
					mSaveGameButton->SetActive(false);
				} else {
					mContinueGameButton->SetActive(true);
					mSaveGameButton->SetActive(true);
				}
			} else {
				mSaveGameButton->SetActive(false);
			}

			// destroy current level if player died
			if (Level::Current()->GetPlayer()) {
				if (Level::Current()->GetPlayer()->IsDead()) {
					Level::DestroyCurrent();
				}
			}
		}

		// check if quick save exists, let player to continue game
		ifstream f("quickSave.save");
		if (f.good()) {
			mContinueGameButton->SetActive(true);
			f.close();
		} else {
			mContinueGameButton->SetActive(Level::Current() == nullptr);
		}

		if (mPage == Page::Authors) {
			mModalWindow->CloseNoAction();
			SetAuthorsPageVisible(true);
		} else {
			SetAuthorsPageVisible(false);
		}

		mGUIOptionsCanvas->SetVisible(mPage == Page::Options);
		mOptionsCommonCanvas->SetVisible(mPage == Page::OptionsCommon);
		mOptionsKeysCanvas->SetVisible(mPage == Page::OptionsKeys);
		mOptionsGraphicsCanvas->SetVisible(mPage == Page::OptionsGraphics);
		mSaveGameButton->SetActive(Level::Current() != nullptr);

		if (mPage == Page::LoadGame) {
			mLoadGameCanvas->SetVisible(true);
			// inactivate all buttons
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mLoadGameSlot[i]->SetActive(false);
			}
			for (int i = 0; i < mSaveFilesList.size(); i++) {
				// activate button associated with file
				mLoadGameSlot[i]->SetActive(true);
				mLoadGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mLoadGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
			}
		} else {
			mLoadGameCanvas->SetVisible(false);
		}

		if (mPage == Page::SaveGame) {
			mSaveGameCanvas->SetVisible(true);
			for (int i = mSaveFilesList.size(); i < mSaveLoadSlotCount; i++) {
				mSaveFilesList.push_back(StringBuilder("Slot") << i << ".save");
			}
			for (int i = 0; i < mSaveFilesList.size(); i++) {
				mSaveGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mSaveGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
			}
		} else {
			mSaveGameCanvas->SetVisible(false);
		}

		mFXAAButton->Update();
		mTextureFiltering->Update();
		mSpotLightShadowMapSize->Update();
		mPointLightShadowMapSize->Update();
		mPointShadowsButton->Update();
		mDirectionalLightShadowMapSize->Update();
		mVolumetricFogButton->Update();
		mFPSButton->Update();
		mSpotShadowsButton->Update();
		mHDRButton->Update();
		mParallaxButton->Update();
		mWindowMode->Update();
		mResolutionList->Update();
		mDynamicDirectionalLightShadows->Update();
		mSoftShadows->Update();
		mBloom->Update();

		mMusic->Play();

		// apply graphics settings
		gShowFPS = mFPSButton->IsChecked();
		ruEngine::SetAnisotropicTextureFiltration(mTextureFiltering->GetCurrentValue() > 0);
		ruEngine::SetIsotropyDegree(mTextureFiltering->GetCurrentValue() > 0 ? mTextureFiltering->GetCurrentValue() : 1);
		ruEngine::EnableSpotLightShadows(mSpotShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mPointShadowsButton->IsChecked());
		ruEngine::SetFXAAEnabled(mFXAAButton->IsChecked());
		ruEngine::SetHDREnabled(mHDRButton->IsChecked());
		ruEngine::SetParallaxEnabled(mParallaxButton->IsChecked());
		ruEngine::SetVolumetricFogEnabled(mVolumetricFogButton->IsChecked());
		ruEngine::SetSpotLightShadowMapSize(atoi(mSpotLightShadowMapSize->GetValueString(mSpotLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetPointLightShadowMapSize(atoi(mPointLightShadowMapSize->GetValueString(mPointLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetDirectionalLightShadowMapSize(atoi(mDirectionalLightShadowMapSize->GetValueString(mDirectionalLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetDirectionalLightDynamicShadows(mDynamicDirectionalLightShadows->IsChecked());

		// sync player controls
		SyncPlayerControls();
	} else {
		if (ruInput::IsKeyHit(ruInput::Key::Esc)) {
			pMainMenu->Show();
		}
	}
}

void Menu::FillListOfSaveFiles() {
	mSaveFilesList.clear();
	// iterate through all files with .save extension
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA("*.save", &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				mSaveFilesList.push_back(fd.cFileName);
			}
		} while (FindNextFileA(hFind, &fd));
		FindClose(hFind);
	}
	// crop array to fit to max count
	while (mSaveFilesList.size() > mSaveLoadSlotCount) {
		mSaveFilesList.pop_back();
	}
}

void Menu::SetPage(Page page, bool hideModalWindow) {
	if (hideModalWindow) {
		mModalWindow->Close();
	}
	mPage = (mPage == page) ? Page::Main : page;
	switch (mPage) {
	case Page::Options: mWindowText->SetText(mLocalization.GetString("captionOptions")); break;
	case Page::OptionsGraphics: mWindowText->SetText(mLocalization.GetString("captionOptionsGraphics")); break;
	case Page::OptionsKeys: mWindowText->SetText(mLocalization.GetString("captionOptionsControls")); break;
	case Page::OptionsCommon: mWindowText->SetText(mLocalization.GetString("captionOptionsCommon")); break;
	case Page::SaveGame: mWindowText->SetText(mLocalization.GetString("captionSaveGame")); break;
	case Page::LoadGame: mWindowText->SetText(mLocalization.GetString("captionLoadGame")); break;
	case Page::Authors: mWindowText->SetText(mLocalization.GetString("captionAuthors")); break;
	default: mWindowText->SetText(" "); break;
	}
}

void Menu::LoadConfig() {
	// Load config
	Parser config;
	config.ParseFile("config.cfg");
	if (!config.Empty()) {
		// small lambda to reduce casting bullshit
		auto ReadKey = [&config](const string & param) { return static_cast<ruInput::Key>(static_cast<int>(config.GetNumber(param))); };
		mMasterVolume->SetValue(config.GetNumber("masterVolume"));
		mMusicVolume->SetValue(config.GetNumber("musicVolume"));
		gMusicVolume = mMusicVolume->GetValue();
		mMouseSensivity->SetValue(config.GetNumber("mouseSens"));
		mFXAAButton->SetEnabled(config.GetNumber("fxaaEnabled") != 0);
		ruEngine::SetFXAAEnabled(mFXAAButton->IsChecked());
		mMoveForwardKey->SetSelected(ReadKey("keyMoveForward"));
		mMoveBackwardKey->SetSelected(ReadKey("keyMoveBackward"));
		mStrafeLeftKey->SetSelected(ReadKey("keyStrafeLeft"));
		mStrafeRightKey->SetSelected(ReadKey("keyStrafeRight"));
		mJumpKey->SetSelected(ReadKey("keyJump"));
		mRunKey->SetSelected(ReadKey("keyRun"));
		mFlashLightKey->SetSelected(ReadKey("keyFlashLight"));
		mInventoryKey->SetSelected(ReadKey("keyInventory"));
		mUseKey->SetSelected(ReadKey("keyUse"));
		mQuickSaveKey->SetSelected(ReadKey("keyQuickSave"));
		gKeyQuickSave = mQuickSaveKey->GetSelectedKey();
		mQuickLoadKey->SetSelected(ReadKey("keyQuickLoad"));
		gKeyQuickLoad = mQuickLoadKey->GetSelectedKey();
		mStealthKey->SetSelected(ReadKey("keyStealth"));
		mSpotShadowsButton->SetEnabled(config.GetNumber("spotShadowsEnabled") != 0);
		mMusic->SetVolume(mMusicVolume->GetValue() / 100.0f);
		mHDRButton->SetEnabled(config.GetNumber("hdrEnabled") != 0);
		mParallaxButton->SetEnabled(config.GetNumber("parallax") != 0);
		mTextureFiltering->SetCurrentValue(config.GetNumber("textureFiltering"));
		mLookLeftKey->SetSelected(ReadKey("keyLookLeft"));
		mLookRightKey->SetSelected(ReadKey("keyLookRight"));
		mFPSButton->SetEnabled(config.GetNumber("showFPS") != 0.0f);
		gShowFPS = mFPSButton->IsChecked();
		mFOVSlider->SetValue(config.GetNumber("fov"));
		mVolumetricFogButton->SetEnabled(config.GetNumber("volumetricFog") != 0);
		mSpotLightShadowMapSize->SetCurrentValue(config.GetNumber("spotLightShadowMapSize"));
		mPointLightShadowMapSize->SetCurrentValue(config.GetNumber("pointLightShadowMapSize"));
		mDirectionalLightShadowMapSize->SetCurrentValue(config.GetNumber("directionalLightShadowMapSize"));
		mDynamicDirectionalLightShadows->SetEnabled(config.GetNumber("dynamicDirectionalLightShadows") != 0);		
		mBloom->SetEnabled(config.GetNumber("bloom") != 0);
		// apply
		ruEngine::SetAnisotropicTextureFiltration(mTextureFiltering->GetCurrentValue());
		ruEngine::SetParallaxEnabled(mParallaxButton->IsChecked());
		ruEngine::SetHDREnabled(mHDRButton->IsChecked());
		ruEngine::EnableSpotLightShadows(mSpotShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mPointShadowsButton->IsChecked());
		ruSound::SetMasterVolume(mMasterVolume->GetValue() / 100.0f);
		ruEngine::SetVolumetricFogEnabled(mVolumetricFogButton->IsChecked());
		ruEngine::SetSpotLightShadowMapSize(atoi(mSpotLightShadowMapSize->GetValueString(mSpotLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetPointLightShadowMapSize(atoi(mPointLightShadowMapSize->GetValueString(mPointLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetDirectionalLightShadowMapSize(atoi(mDirectionalLightShadowMapSize->GetValueString(mDirectionalLightShadowMapSize->GetCurrentValue()).c_str()));
		ruEngine::SetDirectionalLightDynamicShadows(mDynamicDirectionalLightShadows->IsChecked());
	}
}

void Menu::SyncPlayerControls() {
	if (Level::Current()) {
		auto & player = Level::Current()->GetPlayer();
		if (player) {
			player->mKeyMoveForward = mMoveForwardKey->GetSelectedKey();
			player->mKeyMoveBackward = mMoveBackwardKey->GetSelectedKey();
			player->mKeyStrafeLeft = mStrafeLeftKey->GetSelectedKey();
			player->mKeyStrafeRight = mStrafeRightKey->GetSelectedKey();
			player->mKeyJump = mJumpKey->GetSelectedKey();
			player->mKeyRun = mRunKey->GetSelectedKey();
			player->mKeyFlashlightHotkey = mFlashLightKey->GetSelectedKey();
			player->mKeyInventory = mInventoryKey->GetSelectedKey();
			player->mKeyUse = mUseKey->GetSelectedKey();
			player->mKeyStealth = mStealthKey->GetSelectedKey();
			player->mKeyLookLeft = mLookLeftKey->GetSelectedKey();
			player->mKeyLookRight = mLookRightKey->GetSelectedKey();
			player->mFov.SetMin(mFOVSlider->GetValue());
			player->mFov.SetMax(mFOVSlider->GetValue() + 5);
			player->mFov.Set(mFOVSlider->GetValue());
		}
	}
}

void Menu::WriteConfig() {
	ofstream config("config.cfg");
	// small helping lambdas
	auto WriteString = [&config](const string & name, const string & value) { config << name << "=\"" << value << "\";\n"; };
	auto WriteInteger = [&config](const string & name, int value) { config << name << "=\"" << value << "\";\n"; };
	auto WriteFloat = [&config](const string & name, float value) { config << name << "=\"" << value << "\";\n"; };
	auto GetSelectedKey = [](const unique_ptr<WaitKeyButton> & wkb) { return static_cast<int>(wkb->GetSelectedKey()); };
	// write vars
	WriteFloat("mouseSens", mMouseSensivity->GetValue());
	WriteFloat("masterVolume", mMasterVolume->GetValue());
	WriteFloat("musicVolume", mMusicVolume->GetValue());
	WriteInteger("fxaaEnabled", mFXAAButton->IsChecked() ? 1 : 0);
	WriteInteger("keyMoveForward", GetSelectedKey(mMoveForwardKey));
	WriteInteger("keyMoveBackward", GetSelectedKey(mMoveBackwardKey));
	WriteInteger("keyStrafeLeft", GetSelectedKey(mStrafeLeftKey));
	WriteInteger("keyStrafeRight", GetSelectedKey(mStrafeRightKey));
	WriteInteger("keyJump", GetSelectedKey(mJumpKey));
	WriteInteger("keyFlashLight", GetSelectedKey(mFlashLightKey));
	WriteInteger("keyRun", GetSelectedKey(mRunKey));
	WriteInteger("keyInventory", GetSelectedKey(mInventoryKey));
	WriteInteger("keyUse", GetSelectedKey(mUseKey));
	WriteInteger("keyQuickSave", GetSelectedKey(mQuickSaveKey));
	WriteInteger("keyQuickLoad", GetSelectedKey(mQuickLoadKey));
	WriteInteger("spotShadowsEnabled", ruEngine::IsSpotLightShadowsEnabled() ? 1 : 0);
	WriteInteger("hdrEnabled", ruEngine::IsHDREnabled() ? 1 : 0);
	WriteInteger("keyStealth", GetSelectedKey(mStealthKey));
	WriteInteger("textureFiltering", mTextureFiltering->GetCurrentValue());
	WriteInteger("keyLookLeft", GetSelectedKey(mLookLeftKey));
	WriteInteger("keyLookRight", GetSelectedKey(mLookRightKey));
	WriteInteger("showFPS", mFPSButton->IsChecked() ? 1 : 0);
	WriteInteger("parallax", mParallaxButton->IsChecked() ? 1 : 0);
	WriteFloat("fov", mFOVSlider->GetValue());
	WriteFloat("volumetricFog", mVolumetricFogButton->IsChecked() ? 1 : 0);
	WriteInteger("spotLightShadowMapSize", mSpotLightShadowMapSize->GetCurrentValue());
	WriteInteger("pointLightShadowMapSize", mPointLightShadowMapSize->GetCurrentValue());
	WriteInteger("directionalLightShadowMapSize", mDirectionalLightShadowMapSize->GetCurrentValue());
	WriteInteger("dynamicDirectionalLightShadows", mDynamicDirectionalLightShadows->IsChecked() ? 1 : 0);
	WriteInteger("bloom", mBloom->IsChecked() ? 1 : 0);
}

Menu::~Menu() {

}

void Menu::SetMainPageVisible(bool state) {
	mGUIMainButtonsCanvas->SetVisible(state);
}

void Menu::SetAuthorsPageVisible(bool state) {
	mAuthorsBackground->SetVisible(state);
}

bool Menu::IsVisible() {
	return mVisible;
}

void Menu::AnimateCamera() {
	mCameraAnimationOffset = mCameraAnimationOffset.Lerp(mCameraAnimationNewOffset, 0.00065f);
	if ((mCameraAnimationOffset - mCameraAnimationNewOffset).Length2() < 0.015) {
		mCameraAnimationNewOffset = ruVector3(frandom(-0.25, 0.25), frandom(-0.25, 0.25), frandom(-0.25, 0.25));
	}
	mpCamera->mCamera->SetPosition(mCameraInitialPosition + mCameraAnimationOffset);
}
