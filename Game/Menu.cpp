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
	mDistBetweenButtons(72),
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

	const float buttonHeight = 32;
	const float buttonWidth = 128;
	const float buttonXOffset = 10;

	// load textures
	auto texTab = ruTexture::Request("data/gui/menu/tab.tga");
	auto texButton = ruTexture::Request("data/gui/menu/button.tga");
	auto texSmallButton = ruTexture::Request("data/gui/menu/button.tga");

	// Setup
	mGUICanvas = mGUIScene->CreateRect(0, 0, 0, 0, nullptr);
	{
		// Main 
		mGUIMainButtonsCanvas = mGUIScene->CreateRect(20, ruVirtualScreenHeight - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8, texTab, pGUIProp->mBackColor);
		mGUIMainButtonsCanvas->Attach(mGUICanvas);
		{
			mGUIContinueGameButton = mGUIScene->CreateButton(buttonXOffset, 5, buttonWidth, buttonHeight, texButton, mLocalization.GetString("continueButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIContinueGameButton->Attach(mGUIMainButtonsCanvas);
			mGUIContinueGameButton->AddAction(ruGUIAction::OnClick, [this] { OnContinueGameClick(); });

			mGUIStartButton = mGUIScene->CreateButton(buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("startButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIStartButton->Attach(mGUIMainButtonsCanvas);
			mGUIStartButton->AddAction(ruGUIAction::OnClick, [this] { OnStartNewGameClick(); });

			mGUISaveGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("saveButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUISaveGameButton->Attach(mGUIMainButtonsCanvas);
			mGUISaveGameButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::SaveGame); FillListOfSaveFiles(); });

			mGUILoadGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("loadButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUILoadGameButton->Attach(mGUIMainButtonsCanvas);
			mGUILoadGameButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::LoadGame); FillListOfSaveFiles(); });

			mGUIOptionsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("optionsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsButton->Attach(mGUIMainButtonsCanvas);
			mGUIOptionsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::Options); });

			mGUIAuthorsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("authorsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIAuthorsButton->Attach(mGUIMainButtonsCanvas);
			mGUIAuthorsButton->AddAction(ruGUIAction::OnClick, [this] { SetPage(Page::Authors); });

			mGUIExitButton = mGUIScene->CreateButton(buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, texButton, mLocalization.GetString("exitButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIExitButton->Attach(mGUIMainButtonsCanvas);
			mGUIExitButton->AddAction(ruGUIAction::OnClick, [this] { OnExitGameClick(); });
		}

		const int aTabX = 200;
		const int aTabY = ruVirtualScreenHeight - 4.0 * mDistBetweenButtons;
		const int aTabWidth = buttonWidth * 4;
		const int aTabHeight = buttonHeight * 8;

		// Modal window
		mpModalWindow = unique_ptr<ModalWindow>(new ModalWindow(mGUIScene, aTabX, aTabY, aTabWidth, aTabHeight, texTab, texButton, pGUIProp->mBackColor));
		mpModalWindow->AttachTo(mGUICanvas);

		// Page title
		mGUIWindowText = mGUIScene->CreateText(" ", aTabX, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mGUIWindowText->Attach(mGUICanvas);

		// Product name
		mGUICaption = mGUIScene->CreateText("The Mine", 20, aTabY - 17, aTabWidth * 1.5f, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mGUICaption->Attach(mGUICanvas);

		// Options
		mGUIOptionsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mGUICanvas);
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
		mGUIOptionsKeysCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mGUICanvas);
		mGUIOptionsKeysCanvas->SetVisible(false);
		{
			// First column
			float x = 40, y = 10;

			mpMoveForwardKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("forward"));
			mpMoveForwardKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpMoveBackwardKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("backward"));
			mpMoveBackwardKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStrafeLeftKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeLeft"));
			mpStrafeLeftKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStrafeRightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("strafeRight"));
			mpStrafeRightKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpJumpKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("jump"));
			mpJumpKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpFlashLightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("flashLight"));
			mpFlashLightKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpRunKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("run"));
			mpRunKey->AttachTo(mGUIOptionsKeysCanvas);

			// Second column
			x += 150;
			y = 10;
			mpInventoryKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("inventory"));
			mpInventoryKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpUseKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("use"));
			mpUseKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpQuickLoadKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickLoad"));
			mpQuickLoadKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpQuickSaveKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("quickSave"));
			mpQuickSaveKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStealthKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("stealth"));
			mpStealthKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpLookLeftKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookLeft"));
			mpLookLeftKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpLookRightKey = make_unique<WaitKeyButton>(mGUIScene, x, y, texSmallButton, mLocalization.GetString("lookRight"));
			mpLookRightKey->AttachTo(mGUIOptionsKeysCanvas);
		}

		// Options: Graphics
		mGUIOptionsGraphicsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsGraphicsCanvas->Attach(mGUICanvas);
		mGUIOptionsGraphicsCanvas->SetVisible(false);
		{
			const float x = 30, y = 10;
			mpFXAAButton = make_unique<RadioButton>(mGUIScene, x, y, texButton, mLocalization.GetString("fxaa"));
			mpFXAAButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpFPSButton = make_unique<RadioButton>(mGUIScene, x, y + 0.5 * mDistBetweenButtons, texButton, mLocalization.GetString("showFPS"));
			mpFPSButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpShadowsButton = make_unique<RadioButton>(mGUIScene, x, y + mDistBetweenButtons, texButton, mLocalization.GetString("spotLightShadows"));
			mpShadowsButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpHDRButton = make_unique<RadioButton>(mGUIScene, x, y + 1.5 * mDistBetweenButtons, texButton, mLocalization.GetString("hdr"));
			mpHDRButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpParallaxButton = make_unique<RadioButton>(mGUIScene, x, y + 2.0 * mDistBetweenButtons, texButton, mLocalization.GetString("parallax"));
			mpParallaxButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpTextureFiltering = make_unique<ScrollList>(mGUIScene, x, y + 2.5 * mDistBetweenButtons, texButton, mLocalization.GetString("filtering"));
			mpTextureFiltering->AttachTo(mGUIOptionsGraphicsCanvas);
			mpTextureFiltering->AddValue(mLocalization.GetString("trilinear"));
			mpTextureFiltering->AddValue(mLocalization.GetString("anisotropic"));
		}

		// Authors
		mGUIAuthorsBackground = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIAuthorsBackground->Attach(mGUICanvas);
		{
			mGUIAuthorsText = mGUIScene->CreateText(mLocalization.GetString("authorsText"), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
			mGUIAuthorsText->Attach(mGUIAuthorsBackground);
		}

		// Save Game
		mGUISaveGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUISaveGameCanvas->Attach(mGUICanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUISaveGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mGUISaveGameSlot[i]->Attach(mGUISaveGameCanvas);
				mGUISaveGameSlot[i]->AddAction(ruGUIAction::OnClick, [this] { OnCreateSaveClick(); });

				mGUISaveGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left);
				mGUISaveGameFileTime[i]->Attach(mGUISaveGameCanvas);

				y += 1.1f * buttonHeight;
			}
		}

		// Load Game
		mGUILoadGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUILoadGameCanvas->Attach(mGUICanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUILoadGameSlot[i] = mGUIScene->CreateButton(20, y, buttonWidth, buttonHeight, texButton, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mGUILoadGameSlot[i]->Attach(mGUILoadGameCanvas);
				mGUILoadGameSlot[i]->AddAction(ruGUIAction::OnClick, [this] { OnLoadSaveClick(); });

				mGUILoadGameFileTime[i] = mGUIScene->CreateText(" ", buttonWidth + 30, y, 160, buttonHeight, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left); \
					mGUILoadGameFileTime[i]->Attach(mGUILoadGameCanvas);
				y += 1.1f * buttonHeight;
			}
		}

		// Options: Common
		mGUIOptionsCommonCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, texTab, pGUIProp->mBackColor);
		mGUIOptionsCommonCanvas->Attach(mGUICanvas);
		mGUIOptionsCommonCanvas->SetVisible(false);
		{
			const int yOffset = (aTabHeight - 1.5 * mDistBetweenButtons) / 2;
			const int xOffset = aTabWidth / 6.5;

			mFOVSlider = make_unique<Slider>(mGUIScene, xOffset, yOffset - 0.5f * mDistBetweenButtons, 55, 90, 1.0f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("fov"));
			mFOVSlider->AttachTo(mGUIOptionsCommonCanvas);

			mpMasterVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("masterVolume"));
			mpMasterVolume->AttachTo(mGUIOptionsCommonCanvas);
			mpMasterVolume->SetChangeAction([this] { ruSound::SetMasterVolume(mpMasterVolume->GetValue() / 100.0f); });

			mpMusicVolume = make_unique<Slider>(mGUIScene, xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("musicVolume"));
			mpMusicVolume->AttachTo(mGUIOptionsCommonCanvas);
			mpMusicVolume->SetChangeAction([this] { OnMusicVolumeChange(); });

			mpMouseSensivity = make_unique<Slider>(mGUIScene, xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("mouseSens"));
			mpMouseSensivity->AttachTo(mGUIOptionsCommonCanvas);
			mpMouseSensivity->SetChangeAction([this] { gMouseSens = mpMouseSensivity->GetValue() / 100.0f; });
		}
	}

	SetAuthorsPageVisible(false);
	SetPage(Page::Main);
	LoadConfig();
}

float Menu::GetMusicVolume() {
	return mpMusicVolume->GetValue() / 100.0f;
}

void Menu::Show() {
	mpCamera->MakeCurrent();
	mpCamera->FadeIn();
	mScene->Show();
	mGUICaption->SetVisible(true);
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
	mGUICaption->SetVisible(false);
	mGUIWindowText->SetVisible(false);
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
	mpModalWindow->Ask(mLocalization.GetString("newGameQuestion"));
	mpModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { Level::Change(g_initialLevel);	Hide(); }); });
}

void Menu::OnExitGameClick() {
	SetPage(Page::Main);
	mpModalWindow->Ask(mLocalization.GetString("endGameQuestion"));
	mpModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { DoExitGame(); }); });
}

void Menu::OnContinueGameClick() {
	SetPage(Page::Main);
	if (!Level::Current()) {
		mpModalWindow->Ask(mLocalization.GetString("continueLastGameQuestion"));
		mpModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader("quickSave.save").RestoreWorldState(); Hide(); }); });
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
	for (int i = 0; i < mSaveLoadSlotCount; ++i) {
		if (mGUILoadGameSlot[i]->IsHit()) {
			mLoadSaveGameName = mSaveFilesList[i];
		}
	}
	mpModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mLoadSaveGameName << "." << mLocalization.GetString("loadSaveQuestion"));
	mpModalWindow->SetYesAction([this] { CameraStartFadeOut([this] { SaveLoader(mLoadSaveGameName).RestoreWorldState();	Hide(); }); });
}

void Menu::OnCreateSaveClick() {
	for (int i = 0; i < mSaveLoadSlotCount; ++i) {
		if (mGUISaveGameSlot[i]->IsHit()) {
			mSaveGameSlotName = mSaveFilesList[i];
		}
	}
	mpModalWindow->Ask(StringBuilder() << mLocalization.GetString("youSelect") << mSaveGameSlotName << "." << mLocalization.GetString("rewriteSaveQuestion"));
	mpModalWindow->SetYesAction([this] { SaveWriter(mSaveGameSlotName).SaveWorldState(); SetPage(Page::Main); });
}

void Menu::OnMusicVolumeChange() {
	mMusic->SetVolume(mpMusicVolume->GetValue() / 100.0f);
	gMusicVolume = mpMusicVolume->GetValue() / 100.0f;
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
			// destroy current level if player died
			if (Level::Current()->GetPlayer()) {
				if (Level::Current()->GetPlayer()->IsDead()) {
					Level::DestroyCurrent();
				}
			}

			// back to menu by pressing [Esc]
			if (ruInput::IsKeyHit(ruInput::Key::Esc)) {
				CameraStartFadeOut([this] {	Hide(); });
			}

			// switch on/off buttons if player dead or alive
			auto & player = Level::Current()->GetPlayer();
			if (player) {
				if (player->IsDead()) {
					mGUIContinueGameButton->SetActive(false);
					mGUISaveGameButton->SetActive(false);
				} else {
					mGUIContinueGameButton->SetActive(true);
					mGUISaveGameButton->SetActive(true);
				}
			} else {
				mGUISaveGameButton->SetActive(false);
			}
		}

		// check if quick save exists, let player to continue game
		ifstream f("quickSave.save");
		if (f.good()) {
			mGUIContinueGameButton->SetActive(true);
			f.close();
		} else {
			mGUIContinueGameButton->SetActive(Level::Current() == nullptr);
		}

		if (mPage == Page::Authors) {
			mpModalWindow->CloseNoAction();
			SetAuthorsPageVisible(true);
		} else {
			SetAuthorsPageVisible(false);
		}

		mGUIOptionsCanvas->SetVisible(mPage == Page::Options);
		mGUIOptionsCommonCanvas->SetVisible(mPage == Page::OptionsCommon);
		mGUIOptionsKeysCanvas->SetVisible(mPage == Page::OptionsKeys);
		mGUIOptionsGraphicsCanvas->SetVisible(mPage == Page::OptionsGraphics);
		mGUISaveGameButton->SetActive(Level::Current() != nullptr);

		if (mPage == Page::LoadGame) {
			mGUILoadGameCanvas->SetVisible(true);
			// inactivate all buttons
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUILoadGameSlot[i]->SetActive(false);
			}
			for (int i = 0; i < mSaveFilesList.size(); i++) {
				// activate button associated with file
				mGUILoadGameSlot[i]->SetActive(true);
				mGUILoadGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mGUILoadGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
				if (mGUILoadGameSlot[i]->IsHit()) {
					SetPage(Page::Main, false);
					break;
				}
			}
		} else {
			mGUILoadGameCanvas->SetVisible(false);
		}

		if (mPage == Page::SaveGame) {
			mGUISaveGameCanvas->SetVisible(true);
			for (int i = mSaveFilesList.size(); i < mSaveLoadSlotCount; i++) {
				mSaveFilesList.push_back(StringBuilder("Slot") << i << ".save");
			}
			for (int i = 0; i < mSaveFilesList.size(); i++) {
				mGUISaveGameSlot[i]->GetText()->SetText(mSaveFilesList[i].substr(0, mSaveFilesList[i].find_first_of('.')));
				mGUISaveGameFileTime[i]->SetText(GetFileCreationDate(mSaveFilesList[i]));
				if (mGUISaveGameSlot[i]->IsHit()) {
					SetPage(Page::Main, false);
				}
			}
		} else {
			mGUISaveGameCanvas->SetVisible(false);
		}

		mpFXAAButton->Update();
		mpTextureFiltering->Update();
		mpFPSButton->Update();
		mpShadowsButton->Update();
		mpHDRButton->Update();
		mpParallaxButton->Update();

		mMusic->Play();

		// apply graphics settings
		gShowFPS = mpFPSButton->IsChecked();
		ruEngine::SetAnisotropicTextureFiltration(mpTextureFiltering->GetCurrentValue() > 0);
		ruEngine::EnableSpotLightShadows(mpShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mpShadowsButton->IsChecked());
		ruEngine::SetFXAAEnabled(mpFXAAButton->IsChecked());
		ruEngine::SetHDREnabled(mpHDRButton->IsChecked());
		ruEngine::SetParallaxEnabled(mpParallaxButton->IsChecked());

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
		mpModalWindow->Close();
	}
	mPage = (mPage == page) ? Page::Main : page;
	switch (mPage) {
	case Page::Options: mGUIWindowText->SetText(mLocalization.GetString("captionOptions")); break;
	case Page::OptionsGraphics: mGUIWindowText->SetText(mLocalization.GetString("captionOptionsGraphics")); break;
	case Page::OptionsKeys: mGUIWindowText->SetText(mLocalization.GetString("captionOptionsControls")); break;
	case Page::OptionsCommon: mGUIWindowText->SetText(mLocalization.GetString("captionOptionsCommon")); break;
	case Page::SaveGame: mGUIWindowText->SetText(mLocalization.GetString("captionSaveGame")); break;
	case Page::LoadGame: mGUIWindowText->SetText(mLocalization.GetString("captionLoadGame")); break;
	case Page::Authors: mGUIWindowText->SetText(mLocalization.GetString("captionAuthors")); break;
	default: mGUIWindowText->SetText(" "); break;
	}
}

void Menu::LoadConfig() {
	// Load config
	Parser config;
	config.ParseFile("config.cfg");
	if (!config.Empty()) {
		// small lambda to reduce casting bullshit
		auto ReadKey = [&config](const string & param) { return static_cast<ruInput::Key>(static_cast<int>(config.GetNumber(param))); };
		mpMasterVolume->SetValue(config.GetNumber("masterVolume"));
		mpMusicVolume->SetValue(config.GetNumber("musicVolume"));
		gMusicVolume = mpMusicVolume->GetValue();
		mpMouseSensivity->SetValue(config.GetNumber("mouseSens"));
		mpFXAAButton->SetEnabled(config.GetNumber("fxaaEnabled") != 0);
		ruEngine::SetFXAAEnabled(mpFXAAButton->IsChecked());
		mpMoveForwardKey->SetSelected(ReadKey("keyMoveForward"));
		mpMoveBackwardKey->SetSelected(ReadKey("keyMoveBackward"));
		mpStrafeLeftKey->SetSelected(ReadKey("keyStrafeLeft"));
		mpStrafeRightKey->SetSelected(ReadKey("keyStrafeRight"));
		mpJumpKey->SetSelected(ReadKey("keyJump"));
		mpRunKey->SetSelected(ReadKey("keyRun"));
		mpFlashLightKey->SetSelected(ReadKey("keyFlashLight"));
		mpInventoryKey->SetSelected(ReadKey("keyInventory"));
		mpUseKey->SetSelected(ReadKey("keyUse"));
		mpQuickSaveKey->SetSelected(ReadKey("keyQuickSave"));
		gKeyQuickSave = mpQuickSaveKey->GetSelectedKey();
		mpQuickLoadKey->SetSelected(ReadKey("keyQuickLoad"));
		gKeyQuickLoad = mpQuickLoadKey->GetSelectedKey();
		mpStealthKey->SetSelected(ReadKey("keyStealth"));
		mpShadowsButton->SetEnabled(config.GetNumber("spotShadowsEnabled") != 0);
		mMusic->SetVolume(mpMusicVolume->GetValue() / 100.0f);
		mpHDRButton->SetEnabled(config.GetNumber("hdrEnabled") != 0);
		mpParallaxButton->SetEnabled(config.GetNumber("parallax") != 0);
		mpTextureFiltering->SetCurrentValue(config.GetNumber("textureFiltering"));
		mpLookLeftKey->SetSelected(ReadKey("keyLookLeft"));
		mpLookRightKey->SetSelected(ReadKey("keyLookRight"));
		mpFPSButton->SetEnabled(config.GetNumber("showFPS") != 0.0f);
		gShowFPS = mpFPSButton->IsChecked();
		mFOVSlider->SetValue(config.GetNumber("fov"));
		// apply
		ruEngine::SetAnisotropicTextureFiltration(mpTextureFiltering->GetCurrentValue());
		ruEngine::SetParallaxEnabled(mpParallaxButton->IsChecked());
		ruEngine::SetHDREnabled(mpHDRButton->IsChecked());
		ruEngine::EnableSpotLightShadows(mpShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mpShadowsButton->IsChecked());
		ruSound::SetMasterVolume(mpMasterVolume->GetValue() / 100.0f);
	}
}

void Menu::SyncPlayerControls() {
	if (Level::Current()) {
		auto & player = Level::Current()->GetPlayer();
		if (player) {
			player->mKeyMoveForward = mpMoveForwardKey->GetSelectedKey();
			player->mKeyMoveBackward = mpMoveBackwardKey->GetSelectedKey();
			player->mKeyStrafeLeft = mpStrafeLeftKey->GetSelectedKey();
			player->mKeyStrafeRight = mpStrafeRightKey->GetSelectedKey();
			player->mKeyJump = mpJumpKey->GetSelectedKey();
			player->mKeyRun = mpRunKey->GetSelectedKey();
			player->mKeyFlashlightHotkey = mpFlashLightKey->GetSelectedKey();
			player->mKeyInventory = mpInventoryKey->GetSelectedKey();
			player->mKeyUse = mpUseKey->GetSelectedKey();
			player->mKeyStealth = mpStealthKey->GetSelectedKey();
			player->mKeyLookLeft = mpLookLeftKey->GetSelectedKey();
			player->mKeyLookRight = mpLookRightKey->GetSelectedKey();
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
	WriteFloat("mouseSens", mpMouseSensivity->GetValue());
	WriteFloat("masterVolume", mpMasterVolume->GetValue());
	WriteFloat("musicVolume", mpMusicVolume->GetValue());
	WriteInteger("fxaaEnabled", mpFXAAButton->IsChecked() ? 1 : 0);
	WriteInteger("keyMoveForward", GetSelectedKey(mpMoveForwardKey));
	WriteInteger("keyMoveBackward", GetSelectedKey(mpMoveBackwardKey));
	WriteInteger("keyStrafeLeft", GetSelectedKey(mpStrafeLeftKey));
	WriteInteger("keyStrafeRight", GetSelectedKey(mpStrafeRightKey));
	WriteInteger("keyJump", GetSelectedKey(mpJumpKey));
	WriteInteger("keyFlashLight", GetSelectedKey(mpFlashLightKey));
	WriteInteger("keyRun", GetSelectedKey(mpRunKey));
	WriteInteger("keyInventory", GetSelectedKey(mpInventoryKey));
	WriteInteger("keyUse", GetSelectedKey(mpUseKey));
	WriteInteger("keyQuickSave", GetSelectedKey(mpQuickSaveKey));
	WriteInteger("keyQuickLoad", GetSelectedKey(mpQuickLoadKey));
	WriteInteger("spotShadowsEnabled", ruEngine::IsSpotLightShadowsEnabled() ? 1 : 0);
	WriteInteger("hdrEnabled", ruEngine::IsHDREnabled() ? 1 : 0);
	WriteInteger("keyStealth", GetSelectedKey(mpStealthKey));
	WriteInteger("textureFiltering", mpTextureFiltering->GetCurrentValue());
	WriteInteger("keyLookLeft", GetSelectedKey(mpLookLeftKey));
	WriteInteger("keyLookRight", GetSelectedKey(mpLookRightKey));
	WriteInteger("showFPS", mpFPSButton->IsChecked() ? 1 : 0);
	WriteInteger("parallax", mpParallaxButton->IsChecked() ? 1 : 0);
	WriteFloat("fov", mFOVSlider->GetValue());
}

Menu::~Menu() {

}

void Menu::SetMainPageVisible(bool state) {
	mGUIMainButtonsCanvas->SetVisible(state);
}

void Menu::SetAuthorsPageVisible(bool state) {
	mGUIAuthorsBackground->SetVisible(state);
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
