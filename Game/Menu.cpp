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

Menu::Menu(unique_ptr<Game> & game) :
	mGame(game),
	mDistBetweenButtons(68),
	mVisible(true),
	mPage(Page::Main),
	mLoadSaveGameName("") {
	// Load config
	mConfig.Load("config.cfg");
	mLocalization.Load(mGame->GetLocalizationPath() + "menu.loc");
	// load background scene
	mScene = mGame->GetEngine()->GetSceneFactory()->LoadScene("data/maps/menu.scene");
	// create gui scene
	mGUIScene = mGame->GetEngine()->CreateGUIScene();
	// create camera
	mpCamera = make_unique<GameCamera>(mGUIScene);
	mCameraFadeActionDone = false;
	mCameraInitialPosition = mScene->FindChild("Camera")->GetPosition();
	mCameraAnimationNewOffset = Vector3(0.5, 0.5, 0.5);
	
	auto soundSystem = mGame->GetEngine()->GetSoundSystem();

	mPickSound = soundSystem->LoadSound2D("data/sounds/menupick.ogg");
	mMusic = soundSystem->LoadMusic("data/music/menu.ogg");

	const float buttonHeight = 30;
	const float buttonWidth = 128;
	const float buttonXOffset = 10;

	auto renderer = mGame->GetEngine()->GetRenderer();

	// load textures
	auto texTab = renderer->GetTexture("data/gui/menu/tab.tga");
	auto texButton = renderer->GetTexture("data/gui/menu/button.tga");
	auto texSmallButton = renderer->GetTexture("data/gui/menu/button.tga");

	// Setup
	mCanvas = mGUIScene->CreateRect(0, 0, 0, 0, nullptr);
	{
		// Main 
		mGUIMainButtonsCanvas = mGUIScene->CreateRect(20, ruVirtualScreenHeight - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8.5, texTab, pGUIProp->mBackColor);
		mGUIMainButtonsCanvas->Attach(mCanvas);
		{
			mContinueGameButton = mGUIScene->CreateButton(buttonXOffset, 5, buttonWidth, buttonHeight, texButton, mLocalization.GetString("continueButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mContinueGameButton->Attach(mGUIMainButtonsCanvas);
			mContinueGameButton->AddAction(GUIAction::OnClick, [this] { OnContinueGameClick(); });

			mStartButton = mGUIScene->CreateButton(buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("startButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mStartButton->Attach(mGUIMainButtonsCanvas);
			mStartButton->AddAction(GUIAction::OnClick, [this] { OnStartNewGameClick(); });

			mSaveGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("saveButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mSaveGameButton->Attach(mGUIMainButtonsCanvas);
			mSaveGameButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::SaveGame); FillListOfSaveFiles(); });

			mLoadGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("loadButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mLoadGameButton->Attach(mGUIMainButtonsCanvas);
			mLoadGameButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::LoadGame); FillListOfSaveFiles(); });

			mOptionsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("optionsButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mOptionsButton->Attach(mGUIMainButtonsCanvas);
			mOptionsButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::Options); });

			mAuthorsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("authorsButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mAuthorsButton->Attach(mGUIMainButtonsCanvas);
			mAuthorsButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::Authors); });

			mExitButton = mGUIScene->CreateButton(buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("exitButton"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mExitButton->Attach(mGUIMainButtonsCanvas);
			mExitButton->AddAction(GUIAction::OnClick, [this] { OnExitGameClick(); });
		}

		const int aTabX = 200;
		const int aTabY = ruVirtualScreenHeight - 4.0 * mDistBetweenButtons;
		const int aTabWidth = buttonWidth * 4;
		const int aTabHeight = buttonHeight * 8.5;

		// Modal window
		mModalWindow = make_unique<ModalWindow>(mGUIScene, aTabX, aTabY, aTabWidth, aTabHeight, texTab, texButton, pGUIProp->mBackColor);
		mModalWindow->AttachTo(mCanvas);

		// Page title
		mWindowText = mGUIScene->CreateText(" ", aTabX, aTabY - 21, aTabWidth, 32, pGUIProp->mFont, Vector3(255, 255, 255), TextAlignment::Left);
		mWindowText->Attach(mCanvas);

		// Product name
		mCaption = mGUIScene->CreateText("The Mine", 20, aTabY - 21, aTabWidth * 1.5f, 32, pGUIProp->mFont, Vector3(255, 255, 255), TextAlignment::Left);
		mCaption->Attach(mCanvas);

		// Options
		mGUIOptionsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mCanvas);
		{
			const int yOffset = (aTabHeight - 2 * mDistBetweenButtons) / 2;

			mGUIOptionsCommonButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset, buttonWidth, buttonHeight, texButton, mLocalization.GetString("commonSettings"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mGUIOptionsCommonButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsCommonButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::OptionsCommon); });

			mGUIOptionsControlsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("controls"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mGUIOptionsControlsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsControlsButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::OptionsKeys); });

			mGUIOptionsGraphicsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("graphics"), pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
			mGUIOptionsGraphicsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsGraphicsButton->AddAction(GUIAction::OnClick, [this] { SetPage(Page::OptionsGraphics); });
		}

		// Options: Keys
		mOptionsKeysCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mCanvas);
		mOptionsKeysCanvas->SetVisible(false);
		{
			// First column
			float x = 40, y = 10;

			mMoveForwardKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("forward"));
			mMoveForwardKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mMoveBackwardKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("backward"));
			mMoveBackwardKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStrafeLeftKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeLeft"));
			mStrafeLeftKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStrafeRightKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeRight"));
			mStrafeRightKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mJumpKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("jump"));
			mJumpKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mFlashLightKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("flashLight"));
			mFlashLightKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mRunKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("run"));
			mRunKey->AttachTo(mOptionsKeysCanvas);

			// Second column
			x += 150;
			y = 10;
			mInventoryKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("inventory"));
			mInventoryKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mUseKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("use"));
			mUseKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mQuickLoadKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickLoad"));
			mQuickLoadKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mQuickSaveKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickSave"));
			mQuickSaveKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mStealthKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("stealth"));
			mStealthKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mLookLeftKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookLeft"));
			mLookLeftKey->AttachTo(mOptionsKeysCanvas);
			y += 32 * 1.1f;
			mLookRightKey = make_unique<WaitKeyButton>(mGame, mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookRight"));
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

			mDynamicDirectionalLightShadows = make_unique<RadioButton>(mGUIScene, x, y, texButton, mLocalization.GetString("dynamicDirectionalLightShadows"));
			mDynamicDirectionalLightShadows->AttachTo(mOptionsGraphicsCanvas);

			mBloom = make_unique<RadioButton>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("bloom"));
			mBloom->AttachTo(mOptionsGraphicsCanvas);

			mSoftParticles = make_unique<RadioButton>(mGUIScene, x, y + 1.0 * mDistBetweenButtons, texButton, mLocalization.GetString("softParticles"));
			mSoftParticles->AttachTo(mOptionsGraphicsCanvas);

			mVSync = make_unique<RadioButton>(mGUIScene, x, y + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("vsync"));
			mVSync->AttachTo(mOptionsGraphicsCanvas);
			//			mVSync->OnChange += [this] { mSettingsApplied->SetText(mLocalization.GetString("settingsNotApplied")); mSettingsApplied->SetColor(Vector3(255, 0, 0)); };

						// next column
			x += 170;

			mResolutionList = make_unique<ScrollList>(mGUIScene, x, y, texButton, mLocalization.GetString("resolution"));
			mResolutionList->AttachTo(mOptionsGraphicsCanvas);
			auto videoModes = mGame->GetEngine()->GetRenderer()->GetVideoModeList();
			for(auto videoMode : videoModes) {
				mResolutionList->AddValue(StringBuilder() << videoMode.mWidth << "x" << videoMode.mHeight << "@" << videoMode.mRefreshRate);
			}
			mResolutionList->OnChange += [this] { mSettingsApplied->SetText(mLocalization.GetString("settingsNotApplied")); mSettingsApplied->SetColor(Vector3(255, 0, 0)); };

			mWindowMode = make_unique<ScrollList>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("windowMode"));
			mWindowMode->AttachTo(mOptionsGraphicsCanvas);
			mWindowMode->AddValue(mLocalization.GetString("windowed"));
			mWindowMode->AddValue(mLocalization.GetString("fullscreen"));
			mWindowMode->AddValue(mLocalization.GetString("borderless"));
			mWindowMode->OnChange += [this] { mSettingsApplied->SetText(mLocalization.GetString("settingsNotApplied")); mSettingsApplied->SetColor(Vector3(255, 0, 0)); };

			mTextureFiltering = make_unique<ScrollList>(mGUIScene, x, y + 1.0 * mDistBetweenButtons, texButton, mLocalization.GetString("filtering"));
			mTextureFiltering->AttachTo(mOptionsGraphicsCanvas);
			mTextureFiltering->AddValue(mLocalization.GetString("trilinear"));
			for(int i = 1; i <= mGame->GetEngine()->GetRenderer()->GetMaxIsotropyDegree(); ++i) {
				mTextureFiltering->AddValue(StringBuilder() << mLocalization.GetString("anisotropic") << " x" << i);
			}

			mSpotLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("spotLightShadowMap"));
			mSpotLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for(int i = 256; i <= 2048; i *= 2) {
				mSpotLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mPointLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 2.0 * mDistBetweenButtons, texButton, mLocalization.GetString("pointLightShadowMap"));
			mPointLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for(int i = 128; i <= 1024; i *= 2) {
				mPointLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mDirectionalLightShadowMapSize = make_unique<ScrollList>(mGUIScene, x, y + 2.5 * mDistBetweenButtons, texButton, mLocalization.GetString("directionalLightShadowMap"));
			mDirectionalLightShadowMapSize->AttachTo(mOptionsGraphicsCanvas);
			for(int i = 1024; i <= 4096; i *= 2) {
				mDirectionalLightShadowMapSize->AddValue(StringBuilder() << i << " x " << i);
			}

			mSettingsApplied = mGUIScene->CreateText(mLocalization.GetString("settingsApplied"), x, y + 3.1 * mDistBetweenButtons, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, Vector3(0, 255, 0), TextAlignment::Left);
			mSettingsApplied->Attach(mOptionsGraphicsCanvas);
		}

		// Authors
		mAuthorsBackground = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mAuthorsBackground->Attach(mCanvas);
		{
			mGUIAuthorsText = mGUIScene->CreateText(mLocalization.GetString("authorsText"), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, Vector3(255, 255, 255), TextAlignment::Left);
			mGUIAuthorsText->Attach(mAuthorsBackground);
		}

		// Save Game
		mSaveGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mSaveGameCanvas->Attach(mCanvas);
		{
			float y = 10;
			for(int i = 0; i < mSaveLoadSlotCount; i++) {
				mSaveGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
				mSaveGameSlot[i]->Attach(mSaveGameCanvas);
				mSaveGameSlot[i]->AddAction(GUIAction::OnClick, [this] { OnCreateSaveClick(); });

				mSaveGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Left);
				mSaveGameFileTime[i]->Attach(mSaveGameCanvas);

				y += 1.1f * buttonHeight;
			}
		}

		// Load Game
		mLoadGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mLoadGameCanvas->Attach(mCanvas);
		{
			float y = 10;
			for(int i = 0; i < mSaveLoadSlotCount; i++) {
				mLoadGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Center);
				mLoadGameSlot[i]->Attach(mLoadGameCanvas);
				mLoadGameSlot[i]->AddAction(GUIAction::OnClick, [this] { OnLoadSaveClick(); });

				mLoadGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Left);
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

			mFOVSlider = make_unique<Slider>(mGUIScene, xOffset, yOffset - 0.5f * mDistBetweenButtons, 55, 90, 1.0f, renderer->GetTexture("data/gui/menu/smallbutton.tga"), mLocalization.GetString("fov"));
			mFOVSlider->AttachTo(mOptionsCommonCanvas);

			mMasterVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset, 0, 100, 2.5f, renderer->GetTexture("data/gui/menu/smallbutton.tga"), mLocalization.GetString("masterVolume"));
			mMasterVolume->AttachTo(mOptionsCommonCanvas);
			mMasterVolume->SetChangeAction([this] { mGame->GetEngine()->GetSoundSystem()->SetMasterVolume(mMasterVolume->GetValue() / 100.0f); });

			mMusicVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, renderer->GetTexture("data/gui/menu/smallbutton.tga"), mLocalization.GetString("musicVolume"));
			mMusicVolume->AttachTo(mOptionsCommonCanvas);
			mMusicVolume->SetChangeAction([this] { OnMusicVolumeChange(); });

			mMouseSensivity = make_unique<Slider>(mGUIScene, xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, renderer->GetTexture("data/gui/menu/smallbutton.tga"), mLocalization.GetString("mouseSens"));
			mMouseSensivity->AttachTo(mOptionsCommonCanvas);
			mMouseSensivity->SetChangeAction([this] { mGame->SetMouseSensitivity(mMouseSensivity->GetValue() / 100.0f); });

			mLanguage = make_unique<ScrollList>(mGUIScene, yOffset, xOffset + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("language"));
			mLanguage->AttachTo(mOptionsCommonCanvas);
			mLanguage->OnChange += [this] { mConfig.SetString("languagePath", StringBuilder("data/lang/") << mLanguage->GetValueString(mLanguage->GetCurrentValue()) << "/"); };

			int current = 0, i = 0;
			WIN32_FIND_DATAA fi;
			HANDLE h = FindFirstFileExA("data/lang/*.*", FindExInfoStandard, &fi, FindExSearchLimitToDirectories, NULL, 0);
			if(h != INVALID_HANDLE_VALUE) {
				do {
					if(fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						string v = fi.cFileName;
						if(v != "." && v != "..") {
							mLanguage->AddValue(v);
							if(mConfig.GetString("languagePath").rfind(v) != string::npos) {
								current = i;
							}
							++i;
						}
					}
				} while(FindNextFileA(h, &fi));
				FindClose(h);
			}

			mLanguage->SetCurrentValue(current);
		}
	}

	SetAuthorsPageVisible(false);
	SetPage(Page::Main);
	ReadConfig();
}

float Menu::GetMusicVolume() {
	return mMusicVolume->GetValue() / 100.0f;
}

void Menu::Show() {
	mpCamera->MakeCurrent();
	mpCamera->FadeIn();
	mScene->Show();
	mCaption->SetVisible(true);
	if(mGame->GetLevel()) {
		mGame->GetLevel()->Hide();
		auto & player = mGame->GetLevel()->GetPlayer();
		if(player) {
			player->SetBodyVisible(false);
			player->SetHUDVisible(false);
		}
	}
	mVisible = true;
	mGame->GetEngine()->GetRenderer()->SetCursorVisible(true);
	mGUIScene->SetVisible(true);
	mGUIScene->SetOpacity(1.0f);
}

void Menu::Hide() {
	mScene->Hide();
	mCaption->SetVisible(false);
	mWindowText->SetVisible(false);
	if(mGame->GetLevel()) {
		mGame->GetLevel()->Show();
		auto & player = mGame->GetLevel()->GetPlayer();
		if(player) {
			player->mpCamera->MakeCurrent();
			player->SetBodyVisible(true);
			player->SetHUDVisible(true);
		}
	}
	mVisible = false;
	mPage = Page::Main;
	mGame->GetEngine()->GetRenderer()->SetCursorVisible(false);
	mMusic->Pause();
	mGUIScene->SetVisible(false);
}

void Menu::OnStartNewGameClick() {
	SetPage(Page::Main);
	mModalWindow->Ask(mLocalization.GetString("newGameQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { mGame->LoadLevel(mGame->GetNewGameLevel());	Hide(); }); });
}

void Menu::OnExitGameClick() {
	SetPage(Page::Main);
	mModalWindow->Ask(mLocalization.GetString("endGameQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { DoExitGame(); }); });
}

void Menu::OnContinueGameClick() {
	SetPage(Page::Main);
	if(!mGame->GetLevel()) {
		mModalWindow->Ask(mLocalization.GetString("continueLastGameQuestion"));
		mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader("quickSave.save").RestoreWorldState(); Hide(); }); });
	} else {
		CameraStartFadeOut([this] {	Hide(); });
	}
}

void Menu::DoExitGame() {
	if(mGame->GetLevel()) {
		auto & player = mGame->GetLevel()->GetPlayer();
		if(player) {
			if(!player->mDead) {
				SaveWriter("lastGame.save").SaveWorldState();
			}
		}
	}
	WriteConfig();
	mGame->Shutdown();
}

void Menu::ApplySettings() {
	auto renderer = mGame->GetEngine()->GetRenderer();
	mGame->ShowFPS(mFPSButton->IsChecked());
	renderer->SetIsotropyDegree(mTextureFiltering->GetCurrentValue() > 0 ? mTextureFiltering->GetCurrentValue() : 1);
	renderer->SetAnisotropicTextureFiltration(mTextureFiltering->GetCurrentValue());
	renderer->SetParallaxEnabled(mParallaxButton->IsChecked());
	renderer->SetHDREnabled(mHDRButton->IsChecked());
	renderer->SetSpotLightShadowsEnabled(mSpotShadowsButton->IsChecked());
	renderer->SetPointLightShadowsEnabled(mPointShadowsButton->IsChecked());
	mGame->GetEngine()->GetSoundSystem()->SetMasterVolume(mMasterVolume->GetValue() / 100.0f);
	renderer->SetVolumetricFogEnabled(mVolumetricFogButton->IsChecked());
	renderer->SetSpotLightShadowMapSize(atoi(mSpotLightShadowMapSize->GetValueString(mSpotLightShadowMapSize->GetCurrentValue()).c_str()));
	renderer->SetPointLightShadowMapSize(atoi(mPointLightShadowMapSize->GetValueString(mPointLightShadowMapSize->GetCurrentValue()).c_str()));
	renderer->SetDirectionalLightShadowMapSize(atoi(mDirectionalLightShadowMapSize->GetValueString(mDirectionalLightShadowMapSize->GetCurrentValue()).c_str()));
	renderer->SetDirectionalLightDynamicShadows(mDynamicDirectionalLightShadows->IsChecked());
	renderer->SetBloomEnabled(mBloom->IsChecked());
}

void Menu::UpdateCamera() {
	mpCamera->Update();
	if(!mCameraFadeActionDone) {
		mGUIScene->SetOpacity(mGUIScene->GetOpacity() - 0.05f); // disappear smoothly
	}
	if(mpCamera->FadeComplete()) {
		if(!mCameraFadeActionDone) {
			if(mCameraFadeDoneAction) {
				mCameraFadeDoneAction();
			}
			mCameraFadeActionDone = true;
		}
	}
}

void Menu::OnLoadSaveClick() {
	SetPage(Page::Main, false);
	for(int i = 0; i < mSaveLoadSlotCount; ++i) {
		if(mLoadGameSlot[i]->IsHit()) {
			mLoadSaveGameName = mSaveFilesList[i];
		}
	}
	mModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mLoadSaveGameName << "." << mLocalization.GetString("loadSaveQuestion"));
	mModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader(mLoadSaveGameName).RestoreWorldState();	Hide(); }); });
}

void Menu::OnCreateSaveClick() {
	SetPage(Page::Main, false);
	for(int i = 0; i < mSaveLoadSlotCount; ++i) {
		if(mSaveGameSlot[i]->IsHit()) {
			mSaveGameSlotName = mSaveFilesList[i];
		}
	}
	mModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mSaveGameSlotName << "." << mLocalization.GetString("rewriteSaveQuestion"));
	mModalWindow->SetYesAction([this] { SaveWriter(mSaveGameSlotName).SaveWorldState(); SetPage(Page::Main); });
}

void Menu::OnMusicVolumeChange() {
	float value = mMusicVolume->GetValue() / 100.0f;
	mMusic->SetVolume(value);
	mGame->SetMusicVolume(value);
	if(mGame->GetLevel()) {
		mGame->GetLevel()->mMusic->SetVolume(value);
	}
}

void Menu::CameraStartFadeOut(const Delegate & onFadeDoneAction) {
	mpCamera->FadeOut();
	mCameraFadeActionDone = false;
	mCameraFadeDoneAction = onFadeDoneAction;
}

void Menu::Update() {
	WaitKeyButton::UpdateAll();
	UpdateCamera();
	SyncPlayerControls();

	if(mVisible) {
		AnimateCamera();
		mGame->GetEngine()->GetRenderer()->SetAmbientColor(Vector3(0.1, 0.1, 0.1));

		if(mGame->GetLevel()) {
			// back to menu by pressing [Esc]
			if(mGame->GetEngine()->GetInput()->IsKeyHit(IInput::Key::Esc)) {
				CameraStartFadeOut([this] {	Hide(); });
			}

			// switch on/off buttons if player dead or alive
			auto & player = mGame->GetLevel()->GetPlayer();
			if(player) {
				if(player->IsDead()) {
					mContinueGameButton->SetActive(false);
					mSaveGameButton->SetActive(false);
				} else {
					mContinueGameButton->SetActive(true);
					mSaveGameButton->SetActive(true);
				}
			} else {
				mSaveGameButton->SetActive(false);
			}
		}

		// check if quick save exists, let player to continue game
		ifstream f("quickSave.save");
		if(f.good()) {
			mContinueGameButton->SetActive(true);
			f.close();
		} else {
			mContinueGameButton->SetActive(mGame->GetLevel() == nullptr);
		}

		if(mPage == Page::Authors) {
			mModalWindow->CloseNoAction();
			SetAuthorsPageVisible(true);
		} else {
			SetAuthorsPageVisible(false);
		}

		mGUIOptionsCanvas->SetVisible(mPage == Page::Options);
		mOptionsCommonCanvas->SetVisible(mPage == Page::OptionsCommon);
		mOptionsKeysCanvas->SetVisible(mPage == Page::OptionsKeys);
		mOptionsGraphicsCanvas->SetVisible(mPage == Page::OptionsGraphics);
		mSaveGameButton->SetActive(mGame->GetLevel() != nullptr);

		if(mPage == Page::LoadGame) {
			mLoadGameCanvas->SetVisible(true);
			// inactivate all buttons
			for(int i = 0; i < mSaveLoadSlotCount; i++) {
				mLoadGameSlot[i]->SetActive(false);
			}
			for(int i = 0; i < mSaveFilesList.size(); i++) {
				// activate button associated with file
				mLoadGameSlot[i]->SetActive(true);
				mLoadGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mLoadGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
			}
		} else {
			mLoadGameCanvas->SetVisible(false);
		}

		if(mPage == Page::SaveGame) {
			mSaveGameCanvas->SetVisible(true);
			for(int i = mSaveFilesList.size(); i < mSaveLoadSlotCount; i++) {
				mSaveFilesList.push_back(StringBuilder("Slot") << i << ".save");
			}
			for(int i = 0; i < mSaveFilesList.size(); i++) {
				mSaveGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mSaveGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
			}
		} else {
			mSaveGameCanvas->SetVisible(false);
		}

		mMusic->Play();

		// apply graphics settings
		ApplySettings();

		// sync player controls
		SyncPlayerControls();
	} else {
		if(mGame->GetEngine()->GetInput()->IsKeyHit(IInput::Key::Esc)) {
			Show();
		}
	}
}

void Menu::FillListOfSaveFiles() {
	mSaveFilesList.clear();
	// iterate through all files with .save extension
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA("*.save", &fd);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				mSaveFilesList.push_back(fd.cFileName);
			}
		} while(FindNextFileA(hFind, &fd));
		FindClose(hFind);
	}
	// crop array to fit to max count
	while(mSaveFilesList.size() > mSaveLoadSlotCount) {
		mSaveFilesList.pop_back();
	}
}

void Menu::SetPage(Page page, bool hideModalWindow) {
	if(hideModalWindow) {
		mModalWindow->Close();
	}
	mPage = (mPage == page) ? Page::Main : page;
	switch(mPage) {
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

void Menu::ReadConfig() {
	// small lambda to reduce casting bullshit
	auto ReadKey = [this](const string & param) { return static_cast<IInput::Key>(static_cast<int>(mConfig.GetNumber(param))); };
	mMasterVolume->SetValue(mConfig.GetNumber("masterVolume"));
	mMusicVolume->SetValue(mConfig.GetNumber("musicVolume"));
	mGame->SetMusicVolume(mMusicVolume->GetValue());
	mMouseSensivity->SetValue(mConfig.GetNumber("mouseSens"));
	mFXAAButton->SetEnabled(mConfig.GetBoolean("fxaaEnabled"));
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
	mGame->SetQuickSaveKey(mQuickSaveKey->GetSelectedKey());
	mQuickLoadKey->SetSelected(ReadKey("keyQuickLoad"));
	mGame->SetQuickLoadKey(mQuickLoadKey->GetSelectedKey());
	mStealthKey->SetSelected(ReadKey("keyStealth"));
	mSpotShadowsButton->SetEnabled(mConfig.GetBoolean("spotShadowsEnabled"));
	mMusic->SetVolume(mMusicVolume->GetValue() / 100.0f);
	mHDRButton->SetEnabled(mConfig.GetBoolean("hdrEnabled"));
	mParallaxButton->SetEnabled(mConfig.GetBoolean("parallax"));
	mTextureFiltering->SetCurrentValue(mConfig.GetNumber("textureFiltering"));
	mLookLeftKey->SetSelected(ReadKey("keyLookLeft"));
	mLookRightKey->SetSelected(ReadKey("keyLookRight"));
	mFPSButton->SetEnabled(mConfig.GetBoolean("showFPS"));
	mGame->ShowFPS(mFPSButton->IsChecked());
	mFOVSlider->SetValue(mConfig.GetNumber("fov"));
	mVolumetricFogButton->SetEnabled(mConfig.GetBoolean("volumetricFog"));
	mSpotLightShadowMapSize->SetCurrentValue(mConfig.GetNumber("spotLightShadowMapSize"));
	mPointLightShadowMapSize->SetCurrentValue(mConfig.GetNumber("pointLightShadowMapSize"));
	mDirectionalLightShadowMapSize->SetCurrentValue(mConfig.GetNumber("directionalLightShadowMapSize"));
	mDynamicDirectionalLightShadows->SetEnabled(mConfig.GetBoolean("dynamicDirectionalLightShadows"));
	mBloom->SetEnabled(mConfig.GetBoolean("bloom"));
	mWindowMode->SetCurrentValue(mConfig.GetNumber("windowMode"));
	mVSync->SetEnabled(mConfig.GetBoolean("vsync"));
	mPointShadowsButton->SetEnabled(mConfig.GetBoolean("pointShadowsEnabled"));
	mSoftParticles->SetEnabled(mConfig.GetBoolean("softParticles"));

	// read resolution and verify
	{
		string resolution = mConfig.GetString("resolution");
		int num = 0;
		for(auto res : mResolutionList->GetValues()) {
			if(res == resolution) {
				mResolutionList->SetCurrentValue(num);
			}
			++num;
		}
	}
	// apply
	ApplySettings();
}

void Menu::SyncPlayerControls() {
	if(mGame->GetLevel()) {
		auto & player = mGame->GetLevel()->GetPlayer();
		if(player) {
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
	auto GetSelectedKey = [](const unique_ptr<WaitKeyButton> & wkb) { return static_cast<int>(wkb->GetSelectedKey()); };
	mConfig.SetNumber("mouseSens", mMouseSensivity->GetValue());
	mConfig.SetNumber("masterVolume", mMasterVolume->GetValue());
	mConfig.SetNumber("musicVolume", mMusicVolume->GetValue());
	mConfig.SetBoolean("fxaaEnabled", mFXAAButton->IsChecked());
	mConfig.SetNumber("keyMoveForward", GetSelectedKey(mMoveForwardKey));
	mConfig.SetNumber("keyMoveBackward", GetSelectedKey(mMoveBackwardKey));
	mConfig.SetNumber("keyStrafeLeft", GetSelectedKey(mStrafeLeftKey));
	mConfig.SetNumber("keyStrafeRight", GetSelectedKey(mStrafeRightKey));
	mConfig.SetNumber("keyJump", GetSelectedKey(mJumpKey));
	mConfig.SetNumber("keyFlashLight", GetSelectedKey(mFlashLightKey));
	mConfig.SetNumber("keyRun", GetSelectedKey(mRunKey));
	mConfig.SetNumber("keyInventory", GetSelectedKey(mInventoryKey));
	mConfig.SetNumber("keyUse", GetSelectedKey(mUseKey));
	mConfig.SetNumber("keyQuickSave", GetSelectedKey(mQuickSaveKey));
	mConfig.SetNumber("keyQuickLoad", GetSelectedKey(mQuickLoadKey));
	mConfig.SetBoolean("spotShadowsEnabled", mSpotShadowsButton->IsChecked());
	mConfig.SetBoolean("hdrEnabled",mHDRButton->IsChecked());
	mConfig.SetNumber("keyStealth", GetSelectedKey(mStealthKey));
	mConfig.SetNumber("textureFiltering", mTextureFiltering->GetCurrentValue());
	mConfig.SetNumber("keyLookLeft", GetSelectedKey(mLookLeftKey));
	mConfig.SetNumber("keyLookRight", GetSelectedKey(mLookRightKey));
	mConfig.SetBoolean("showFPS", mFPSButton->IsChecked());
	mConfig.SetBoolean("parallax", mParallaxButton->IsChecked());
	mConfig.SetNumber("fov", mFOVSlider->GetValue());
	mConfig.SetBoolean("volumetricFog", mVolumetricFogButton->IsChecked());
	mConfig.SetNumber("spotLightShadowMapSize", mSpotLightShadowMapSize->GetCurrentValue());
	mConfig.SetNumber("pointLightShadowMapSize", mPointLightShadowMapSize->GetCurrentValue());
	mConfig.SetNumber("directionalLightShadowMapSize", mDirectionalLightShadowMapSize->GetCurrentValue());
	mConfig.SetBoolean("dynamicDirectionalLightShadows", mDynamicDirectionalLightShadows->IsChecked());
	mConfig.SetNumber("bloom", mBloom->IsChecked());
	mConfig.SetBoolean("windowMode", mWindowMode->GetCurrentValue());
	mConfig.SetBoolean("vsync", mVSync->IsChecked());
	mConfig.SetString("resolution", mResolutionList->GetValueString(mResolutionList->GetCurrentValue()));
	mConfig.SetBoolean("pointShadowsEnabled", mPointShadowsButton->IsChecked());
	mConfig.SetBoolean("softParticles", mSoftParticles->IsChecked());
	mConfig.Save();
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
	if((mCameraAnimationOffset - mCameraAnimationNewOffset).Length2() < 0.015) {
		mCameraAnimationNewOffset = Vector3(frandom(-0.25, 0.25), frandom(-0.25, 0.25), frandom(-0.25, 0.25));
	}
	mpCamera->mCamera->SetPosition(mCameraInitialPosition + mCameraAnimationOffset);
}
