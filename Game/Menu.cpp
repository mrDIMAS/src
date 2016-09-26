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
bool g_continueGame = false;

Menu::Menu() {
	mLocalization.ParseFile(gLocalizationPath + "menu.loc");

	// load background scene
	mScene = ruSceneNode::LoadFromFile("data/maps/menu.scene");

	// create gui scene
	mGUIScene = ruGUIScene::Create();

	mDistBetweenButtons = 72;
	mVisible = true;
	mPage = Page::Main;
	mLoadSaveGameName = "";

	CreateCamera();
	LoadSounds();
	LoadTextures();

	const float buttonHeight = 32;
	const float buttonWidth = 128;
	const float buttonXOffset = 10;

	shared_ptr<ruTexture> tabTexture = ruTexture::Request("data/gui/menu/tab.tga");

	// Setup
	mGUICanvas = mGUIScene->CreateRect(0, 0, 0, 0, nullptr);
	{
		mGUIMainButtonsCanvas = mGUIScene->CreateRect(20, ruVirtualScreenHeight - 4.0 * mDistBetweenButtons, buttonWidth + 2 * buttonXOffset, buttonHeight * 8, tabTexture, pGUIProp->mBackColor);
		mGUIMainButtonsCanvas->Attach(mGUICanvas);
		{
			mGUIContinueGameButton = mGUIScene->CreateButton(buttonXOffset, 5, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("continueButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIContinueGameButton->Attach(mGUIMainButtonsCanvas);
			mGUIContinueGameButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnContinueGameClick));

			mGUIStartButton = mGUIScene->CreateButton(buttonXOffset, 5 + 0.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("startButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIStartButton->Attach(mGUIMainButtonsCanvas);
			mGUIStartButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnStartNewGameClick));

			mGUISaveGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("saveButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUISaveGameButton->Attach(mGUIMainButtonsCanvas);

			mGUILoadGameButton = mGUIScene->CreateButton(buttonXOffset, 5 + 1.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("loadButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUILoadGameButton->Attach(mGUIMainButtonsCanvas);

			mGUIOptionsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("optionsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsButton->Attach(mGUIMainButtonsCanvas);
			mGUIOptionsButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnOptionsClick));

			mGUIAuthorsButton = mGUIScene->CreateButton(buttonXOffset, 5 + 2.5f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("authorsButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIAuthorsButton->Attach(mGUIMainButtonsCanvas);

			mGUIExitButton = mGUIScene->CreateButton(buttonXOffset, 5 + 3.0f * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("exitButton"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIExitButton->Attach(mGUIMainButtonsCanvas);
			mGUIExitButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnExitGameClick));
		}

		int aTabX = 200;
		int aTabY = ruVirtualScreenHeight - 4.0 * mDistBetweenButtons;
		int aTabWidth = buttonWidth * 3;
		int aTabHeight = buttonHeight * 8;

		mpModalWindow = unique_ptr<ModalWindow>(new ModalWindow(mGUIScene, aTabX, aTabY, aTabWidth, aTabHeight, ruTexture::Request("data/gui/menu/tab.tga"), mButtonImage, pGUIProp->mBackColor));
		mpModalWindow->AttachTo(mGUICanvas);

		mGUIWindowText = mGUIScene->CreateText(" ", aTabX, aTabY - 17, aTabWidth, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mGUIWindowText->Attach(mGUICanvas);

		mGUICaption = mGUIScene->CreateText("The Mine", 20, aTabY - 17, aTabWidth * 1.5f, 32, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
		mGUICaption->Attach(mGUICanvas);

		mGUIOptionsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mGUICanvas);
		{
			int yOffset = (aTabHeight - 2 * mDistBetweenButtons) / 2;

			mGUIOptionsCommonButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("commonSettings"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsCommonButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsCommonButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnOptionsCommonClick));

			mGUIOptionsControlsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + 0.5 * mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("controls"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsControlsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsControlsButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnOptionsControlsClick));

			mGUIOptionsGraphicsButton = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, yOffset + mDistBetweenButtons, buttonWidth, buttonHeight, mButtonImage, mLocalization.GetString("graphics"), pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
			mGUIOptionsGraphicsButton->Attach(mGUIOptionsCanvas);
			mGUIOptionsGraphicsButton->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnOptionsGraphicsClick));
		}

		mGUIOptionsKeysCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUIOptionsCanvas->Attach(mGUICanvas);
		mGUIOptionsKeysCanvas->SetVisible(false);
		{
			float x = 40, y = 10;
			mpMoveForwardKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("forward")));
			mpMoveForwardKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpMoveBackwardKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("backward")));
			mpMoveBackwardKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStrafeLeftKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("strafeLeft")));
			mpStrafeLeftKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStrafeRightKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("strafeRight")));
			mpStrafeRightKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpJumpKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("jump")));
			mpJumpKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpFlashLightKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("flashLight")));
			mpFlashLightKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpRunKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("run")));
			mpRunKey->AttachTo(mGUIOptionsKeysCanvas);
			// Second column
			x += 150;
			y = 10;
			mpInventoryKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("inventory")));
			mpInventoryKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpUseKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("use")));
			mpUseKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpQuickLoadKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("quickLoad")));
			mpQuickLoadKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpQuickSaveKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("quickSave")));
			mpQuickSaveKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpStealthKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("stealth")));
			mpStealthKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpLookLeftKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("lookLeft")));
			mpLookLeftKey->AttachTo(mGUIOptionsKeysCanvas);
			y += 32 * 1.1f;
			mpLookRightKey = unique_ptr<WaitKeyButton>(new WaitKeyButton(mGUIScene, x, y, mSmallButtonImage, mLocalization.GetString("lookRight")));
			mpLookRightKey->AttachTo(mGUIOptionsKeysCanvas);
		}

		mGUIOptionsGraphicsCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUIOptionsGraphicsCanvas->Attach(mGUICanvas);
		mGUIOptionsGraphicsCanvas->SetVisible(false);
		{
			float x = 30, y = 10;
			mpFXAAButton = unique_ptr<RadioButton>(new RadioButton(mGUIScene, x, y, mButtonImage, mLocalization.GetString("fxaa")));
			mpFXAAButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpFPSButton = unique_ptr<RadioButton>(new RadioButton(mGUIScene, x, y + 0.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString("showFPS")));
			mpFPSButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpShadowsButton = unique_ptr<RadioButton>(new RadioButton(mGUIScene, x, y + mDistBetweenButtons, mButtonImage, mLocalization.GetString("spotLightShadows")));
			mpShadowsButton->AttachTo(mGUIOptionsGraphicsCanvas);

			mpHDRButton = unique_ptr<RadioButton>(new RadioButton(mGUIScene, x, y + 1.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString("hdr")));
			mpHDRButton->AttachTo(mGUIOptionsGraphicsCanvas);
			mpHDRButton->SetChangeAction(ruDelegate::Bind(this, &Menu::OnHDRButtonClick));

			mpParallaxButton = unique_ptr<RadioButton>(new RadioButton(mGUIScene, x, y + 2.0 * mDistBetweenButtons, mButtonImage, mLocalization.GetString("parallax")));
			mpParallaxButton->AttachTo(mGUIOptionsGraphicsCanvas);
			mpParallaxButton->SetChangeAction(ruDelegate::Bind(this, &Menu::OnParallaxButtonClick));

			mpTextureFiltering = unique_ptr<ScrollList>(new ScrollList(mGUIScene, x, y + 2.5 * mDistBetweenButtons, mButtonImage, mLocalization.GetString("filtering")));
			mpTextureFiltering->AttachTo(mGUIOptionsGraphicsCanvas);
			mpTextureFiltering->AddValue(mLocalization.GetString("trilinear"));
			mpTextureFiltering->AddValue(mLocalization.GetString("anisotropic"));
		}

		mGUIAuthorsBackground = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUIAuthorsBackground->Attach(mGUICanvas);
		{
			mGUIAuthorsText = mGUIScene->CreateText(mLocalization.GetString("authorsText"), 15, 15, aTabWidth - 30, aTabHeight - 30, pGUIProp->mFont, ruVector3(255, 255, 255), ruTextAlignment::Left);
			mGUIAuthorsText->Attach(mGUIAuthorsBackground);
		}

		mGUISaveGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUISaveGameCanvas->Attach(mGUICanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUISaveGameSlot[i] = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mGUISaveGameSlot[i]->Attach(mGUISaveGameCanvas);
				mGUISaveGameSlot[i]->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnSaveClick));
				y += 1.1f * buttonHeight;
			}
		}

		mGUILoadGameCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUILoadGameCanvas->Attach(mGUICanvas);
		{
			float y = 10;
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUILoadGameSlot[i] = mGUIScene->CreateButton((aTabWidth - buttonWidth) / 2, y, buttonWidth, buttonHeight, mButtonImage, "Empty slot", pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Center);
				mGUILoadGameSlot[i]->Attach(mGUILoadGameCanvas);
				mGUILoadGameSlot[i]->AddAction(ruGUIAction::OnClick, ruDelegate::Bind(this, &Menu::OnLoadSaveClick));
				y += 1.1f * buttonHeight;
			}
		}

		mGUIOptionsCommonCanvas = mGUIScene->CreateRect(aTabX, aTabY, aTabWidth, aTabHeight, tabTexture, pGUIProp->mBackColor);
		mGUIOptionsCommonCanvas->Attach(mGUICanvas);
		mGUIOptionsCommonCanvas->SetVisible(false);
		{
			int yOffset = (aTabHeight - 1.5 * mDistBetweenButtons) / 2;
			int xOffset = aTabWidth / 6.5;

			mFOVSlider = unique_ptr<Slider>(new Slider(mGUIScene, xOffset, yOffset - 0.5f * mDistBetweenButtons, 55, 90, 1.0f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("fov")));
			mFOVSlider->AttachTo(mGUIOptionsCommonCanvas);
			mFOVSlider->SetChangeAction(ruDelegate::Bind(this, &Menu::OnFovChanged));

			mpMasterVolume = unique_ptr<Slider>(new Slider(mGUIScene, xOffset, yOffset, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("masterVolume")));
			mpMasterVolume->AttachTo(mGUIOptionsCommonCanvas);
			mpMasterVolume->SetChangeAction(ruDelegate::Bind(this, &Menu::OnSoundVolumeChange));

			mpMusicVolume = unique_ptr<Slider>(new Slider(mGUIScene, xOffset, yOffset + 0.5f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("musicVolume")));
			mpMusicVolume->AttachTo(mGUIOptionsCommonCanvas);
			mpMusicVolume->SetChangeAction(ruDelegate::Bind(this, &Menu::OnMusicVolumeChange));

			mpMouseSensivity = unique_ptr<Slider>(new Slider(mGUIScene, xOffset, yOffset + 1.0f * mDistBetweenButtons, 0, 100, 2.5f, ruTexture::Request("data/gui/menu/smallbutton.tga"), mLocalization.GetString("mouseSens")));
			mpMouseSensivity->AttachTo(mGUIOptionsCommonCanvas);
			mpMouseSensivity->SetChangeAction(ruDelegate::Bind(this, &Menu::OnMouseSensivityChange));
		}
	}

	ruEngine::SetHDREnabled(mpHDRButton->IsChecked());

	SetOptionsPageVisible(false);
	SetAuthorsPageVisible(false);
	SetPage(Page::Main);
	LoadConfig();

	mpCamera->mCamera->SetSkybox(
		ruTexture::Request("data/textures/skyboxes/night3/nightsky_u.jpg"),
		ruTexture::Request("data/textures/skyboxes/night3/nightsky_l.jpg"),
		ruTexture::Request("data/textures/skyboxes/night3/nightsky_r.jpg"),
		ruTexture::Request("data/textures/skyboxes/night3/nightsky_f.jpg"),
		ruTexture::Request("data/textures/skyboxes/night3/nightsky_b.jpg")
	);
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
	mpModalWindow->SetYesAction(ruDelegate::Bind(this, &Menu::StartNewGame));

}

void Menu::OnExitGameClick() {
	SetPage(Page::Main);
	mpModalWindow->Ask(mLocalization.GetString("endGameQuestion"));
	mpModalWindow->SetYesAction(ruDelegate::Bind(this, &Menu::StartExitGame));
}

void Menu::StartNewGame() {
	CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoStartNewGame));
}

void Menu::StartExitGame() {
	CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoExitGame));
}

void Menu::OnContinueGameClick() {
	SetPage(Page::Main);
	if (!Level::Current()) {
		mpModalWindow->Ask(mLocalization.GetString("continueLastGameQuestion"));
		mpModalWindow->SetYesAction(ruDelegate::Bind(this, &Menu::StartContinueGameFromLast));
	} else {
		CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoContinueGameCurrent));
	}
}

void Menu::DoContinueGameCurrent() {
	Hide();
}

void Menu::DoStartNewGame() {
	Level::Change(g_initialLevel);
	Hide();
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

void Menu::OnOptionsClick() {
	SetPage(Page::Options);
}

void Menu::OnOptionsGraphicsClick() {
	SetPage(Page::OptionsGraphics);
}

void Menu::OnOptionsCommonClick() {
	SetPage(Page::OptionsCommon);
}

void Menu::OnOptionsControlsClick() {
	SetPage(Page::OptionsKeys);
}

void Menu::StartContinueGameFromLast() {
	CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoContinueGameFromLast));
}

void Menu::DoContinueGameFromLast() {
	SaveLoader("quickSave.save").RestoreWorldState();
	Hide();
}

void Menu::UpdateCamera() {
	mpCamera->Update();
	if (!mCameraFadeActionDone) {
		mGUIScene->SetOpacity(mGUIScene->GetOpacity() - 0.05f); // disappear smoothly
	}
	if (mpCamera->FadeComplete()) {
		if (!mCameraFadeActionDone) {
			mCameraFadeDoneAction.Call();
			mCameraFadeActionDone = true;
		}
	}
}

void Menu::StartLoadFromSave() {
	CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoLoadFromSave));
}

void Menu::OnLoadSaveClick() {
	mpModalWindow->Ask(mLocalization.GetString("youSelect") + mLoadSaveGameName + "." + mLocalization.GetString("loadSaveQuestion"));
	mpModalWindow->SetYesAction(ruDelegate::Bind(this, &Menu::StartLoadFromSave));
}

void Menu::DoLoadFromSave() {
	SaveLoader(mLoadSaveGameName).RestoreWorldState();
	Hide();
}

void Menu::OnSaveClick() {
	mpModalWindow->Ask(mLocalization.GetString("youSelect") + mSaveGameSlotName + "." + mLocalization.GetString("rewriteSaveQuestion"));
	mpModalWindow->SetYesAction(ruDelegate::Bind(this, &Menu::DoSaveCurrentGame));
}

void Menu::OnMouseSensivityChange() {
	gMouseSens = mpMouseSensivity->GetValue() / 100.0f;
}

void Menu::OnMusicVolumeChange() {
	mMusic->SetVolume(mpMusicVolume->GetValue() / 100.0f);
	gMusicVolume = mpMusicVolume->GetValue() / 100.0f;
	if (Level::Current()) {
		Level::Current()->mMusic->SetVolume(gMusicVolume);
	}
}
void Menu::OnSoundVolumeChange() {
	ruSound::SetMasterVolume(mpMasterVolume->GetValue() / 100.0f);
}

void Menu::OnFovChanged() {
	if (Level::Current()) {
		if (Level::Current()->GetPlayer()) {
			Level::Current()->GetPlayer()->mFov.SetMin(mFOVSlider->GetValue());
			Level::Current()->GetPlayer()->mFov.SetMax(mFOVSlider->GetValue() + 5);
			Level::Current()->GetPlayer()->mFov.Set(mFOVSlider->GetValue());
		}
	}
}

void Menu::DoSaveCurrentGame() {
	SaveWriter(mSaveGameSlotName).SaveWorldState();
	SetPage(Page::Main);
}

void Menu::CameraStartFadeOut(const ruDelegate & onFadeDoneAction) {
	mpCamera->FadeOut();
	mCameraFadeActionDone = false;
	mCameraFadeDoneAction = onFadeDoneAction;
}

void Menu::Update() {
	ruEngine::SetAmbientColor(ruVector3(0.1, 0.1, 0.1));

	WaitKeyButton::UpdateAll();
	UpdateCamera();

	SyncPlayerControls();

	if (mVisible) {

		// destroy current level if player died
		if (Level::Current()) {
			if (Level::Current()->GetPlayer()) {
				if (Level::Current()->GetPlayer()->IsDead()) {
					Level::DestroyCurrent();
				}
			}
		}

		ifstream f("quickSave.save");
		if (f.good()) {
			mGUIContinueGameButton->SetActive(true);
			f.close();
		} else {
			if (!Level::Current()) {
				mGUIContinueGameButton->SetActive(false);
			} else {
				mGUIContinueGameButton->SetActive(true);
			}
		}

		if (ruInput::IsKeyHit(ruInput::Key::Esc)) {
			if (Level::Current()) {
				CameraStartFadeOut(ruDelegate::Bind(this, &Menu::DoContinueGameCurrent));
			}
		}

		CameraFloating();

		if (Level::Current()) {
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

		if (mPage == Page::Authors) {
			mpModalWindow->CloseNoAction();
			SetAuthorsPageVisible(true);
		} else {
			SetAuthorsPageVisible(false);
		}

		if (mPage == Page::Options) {
			SetOptionsPageVisible(true);
		} else {
			SetOptionsPageVisible(false);
		}

		if (mPage == Page::OptionsCommon) {
			mGUIOptionsCommonCanvas->SetVisible(true);
		} else {
			mGUIOptionsCommonCanvas->SetVisible(false);
		}

		if (mPage == Page::LoadGame) {
			mGUILoadGameCanvas->SetVisible(true);
			vector< string > nameList;
			GetFilesWithExtension("*.save", nameList);
			int count = nameList.size();
			if (count >= mSaveLoadSlotCount) {
				count = mSaveLoadSlotCount;
			}
			// inactivate all buttons
			for (int i = 0; i < mSaveLoadSlotCount; i++) {
				mGUILoadGameSlot[i]->SetActive(false);
			}
			for (int i = 0; i < count; i++) {
				// activate button associated with file
				mGUILoadGameSlot[i]->SetActive(true);
				mGUILoadGameSlot[i]->GetText()->SetText(nameList[i]);
				if (mGUILoadGameSlot[i]->IsHit()) {
					mLoadSaveGameName = nameList[i];
					SetPage(Page::Main, false);
				}
			}
		} else {
			mGUILoadGameCanvas->SetVisible(false);
		}

		if (mPage == Page::SaveGame) {
			mGUISaveGameCanvas->SetVisible(true);

			vector< string > nameList;
			GetFilesWithExtension("*.save", nameList);
			for (int iName = nameList.size() - 1; iName < 6; iName++) {
				string saveName = "Slot";
				saveName += ((char)iName + (char)'0');
				saveName += ".save";
				nameList.push_back(saveName);
			}
			int count = nameList.size();
			if (count >= mSaveLoadSlotCount) {
				count = mSaveLoadSlotCount;
			}
			for (int iName = 0; iName < count; iName++) {
				mGUISaveGameSlot[iName]->GetText()->SetText(nameList[iName]);
				if (mGUISaveGameSlot[iName]->IsHit()) {
					mSaveGameSlotName = nameList[iName];
				}
			}
		} else {
			mGUISaveGameCanvas->SetVisible(false);
		}

		if (mPage == Page::OptionsGraphics) {
			mGUIOptionsGraphicsCanvas->SetVisible(true);

			mpFXAAButton->Update();
			mpTextureFiltering->Update();
			mpFPSButton->Update();
			mpShadowsButton->Update();
			mpHDRButton->Update();
			mpParallaxButton->Update();
		} else {
			mGUIOptionsGraphicsCanvas->SetVisible(false);
		}

		if (mPage == Page::OptionsKeys) {
			mGUIOptionsKeysCanvas->SetVisible(true);
		} else {
			mGUIOptionsKeysCanvas->SetVisible(false);
		}

		mMusic->Play();

		if (mGUILoadGameButton->IsHit()) {
			SetPage(Page::LoadGame);
		}
		if (mGUISaveGameButton->IsHit()) {
			SetPage(Page::SaveGame);
		}
		if (mGUIAuthorsButton->IsHit()) {
			SetPage(Page::Authors);
		}

		// Apply graphics settings
		gShowFPS = mpFPSButton->IsChecked();
		if (mpTextureFiltering->GetCurrentValue() == 0) {
			ruEngine::SetAnisotropicTextureFiltration(false);
		} else {
			ruEngine::SetAnisotropicTextureFiltration(true);
		}
		ruEngine::EnableSpotLightShadows(mpShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mpShadowsButton->IsChecked());
		ruEngine::SetFXAAEnabled(mpFXAAButton->IsChecked());

		// sync player controls
		SyncPlayerControls();
	} else {
		if (ruInput::IsKeyHit(ruInput::Key::Esc)) {
			pMainMenu->Show();
		}
	}
}

void Menu::SetPage(Page page, bool hideModalWindow) {
	if (hideModalWindow) {
		mpModalWindow->Close();
	}

	if (mPage == page) {
		mPage = Page::Main;
	} else {
		mPage = page;
	}
	mGUIWindowText->SetText(" ");
	if (mPage == Page::Options) {
		mGUIWindowText->SetText(mLocalization.GetString("captionOptions"));
	}
	if (mPage == Page::OptionsGraphics) {
		mGUIWindowText->SetText(mLocalization.GetString("captionOptionsGraphics"));
	}
	if (mPage == Page::OptionsKeys) {
		mGUIWindowText->SetText(mLocalization.GetString("captionOptionsControls"));
	}
	if (mPage == Page::OptionsCommon) {
		mGUIWindowText->SetText(mLocalization.GetString("captionOptionsCommon"));
	}
	if (mPage == Page::SaveGame) {
		mGUIWindowText->SetText(mLocalization.GetString("captionSaveGame"));
	}
	if (mPage == Page::LoadGame) {
		mGUIWindowText->SetText(mLocalization.GetString("captionLoadGame"));
	}
	if (mPage == Page::Authors) {
		mGUIWindowText->SetText(mLocalization.GetString("captionAuthors"));
	}
}

void Menu::LoadTextures() {
	mButtonImage = ruTexture::Request("data/gui/menu/button.tga");
	mSmallButtonImage = ruTexture::Request("data/gui/menu/button.tga");
}

void Menu::CreateCamera() {
	mpCamera = unique_ptr<GameCamera>(new GameCamera(mGUIScene));
	mCameraFadeActionDone = false;
	mCameraInitialPosition = mScene->FindChild("Camera")->GetPosition();
	mCameraAnimationNewOffset = ruVector3(0.5, 0.5, 0.5);
}

void Menu::LoadSounds() {
	mPickSound = ruSound::Load2D("data/sounds/menupick.ogg");
	mMusic = ruSound::LoadMusic("data/music/menu.ogg");
}

void Menu::LoadConfig() {
	// Load config
	Parser config;
	config.ParseFile("config.cfg");

	if (!config.Empty()) {
		mpMasterVolume->SetValue(config.GetNumber("masterVolume"));

		mpMusicVolume->SetValue(config.GetNumber("musicVolume"));
		gMusicVolume = mpMusicVolume->GetValue();

		mpMouseSensivity->SetValue(config.GetNumber("mouseSens"));
		mpFXAAButton->SetEnabled(config.GetNumber("fxaaEnabled") != 0);
		ruEngine::SetFXAAEnabled(mpFXAAButton->IsChecked());
		mpMoveForwardKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyMoveForward"))));
		mpMoveBackwardKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyMoveBackward"))));
		mpStrafeLeftKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyStrafeLeft"))));
		mpStrafeRightKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyStrafeRight"))));
		mpJumpKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyJump"))));
		mpRunKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyRun"))));
		mpFlashLightKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyFlashLight"))));
		mpInventoryKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyInventory"))));
		mpUseKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyUse"))));

		mpQuickSaveKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyQuickSave"))));
		gKeyQuickSave = mpQuickSaveKey->GetSelectedKey();

		mpQuickLoadKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyQuickLoad"))));
		gKeyQuickLoad = mpQuickLoadKey->GetSelectedKey();

		mpStealthKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyStealth"))));

		mpShadowsButton->SetEnabled(config.GetNumber("spotShadowsEnabled") != 0);
		ruEngine::EnableSpotLightShadows(mpShadowsButton->IsChecked());
		ruEngine::EnablePointLightShadows(mpShadowsButton->IsChecked());

		ruSound::SetMasterVolume(mpMasterVolume->GetValue() / 100.0f);
		mMusic->SetVolume(mpMusicVolume->GetValue() / 100.0f);

		mpHDRButton->SetEnabled(config.GetNumber("hdrEnabled") != 0);
		ruEngine::SetHDREnabled(mpHDRButton->IsChecked());

		mpParallaxButton->SetEnabled(config.GetNumber("parallax") != 0);
		ruEngine::SetParallaxEnabled(mpParallaxButton->IsChecked());

		mpTextureFiltering->SetCurrentValue(config.GetNumber("textureFiltering"));
		ruEngine::SetAnisotropicTextureFiltration(mpTextureFiltering->GetCurrentValue());

		mpLookLeftKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyLookLeft"))));
		mpLookRightKey->SetSelected(static_cast<ruInput::Key>(static_cast<int>(config.GetNumber("keyLookRight"))));

		mpFPSButton->SetEnabled(config.GetNumber("showFPS") != 0.0f);
		gShowFPS = mpFPSButton->IsChecked();

		mFOVSlider->SetValue(config.GetNumber("fov"));
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
			player->mKeyFlashLight = mpFlashLightKey->GetSelectedKey();
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
	WriteFloat(config, "mouseSens", mpMouseSensivity->GetValue());
	WriteFloat(config, "masterVolume", mpMasterVolume->GetValue());
	WriteFloat(config, "musicVolume", mpMusicVolume->GetValue());
	WriteInteger(config, "fxaaEnabled", mpFXAAButton->IsChecked() ? 1 : 0);
	WriteInteger(config, "keyMoveForward", static_cast<int>(mpMoveForwardKey->GetSelectedKey()));
	WriteInteger(config, "keyMoveBackward", static_cast<int>(mpMoveBackwardKey->GetSelectedKey()));
	WriteInteger(config, "keyStrafeLeft", static_cast<int>(mpStrafeLeftKey->GetSelectedKey()));
	WriteInteger(config, "keyStrafeRight", static_cast<int>(mpStrafeRightKey->GetSelectedKey()));
	WriteInteger(config, "keyJump", static_cast<int>(mpJumpKey->GetSelectedKey()));
	WriteInteger(config, "keyFlashLight", static_cast<int>(mpFlashLightKey->GetSelectedKey()));
	WriteInteger(config, "keyRun", static_cast<int>(mpRunKey->GetSelectedKey()));
	WriteInteger(config, "keyInventory", static_cast<int>(mpInventoryKey->GetSelectedKey()));
	WriteInteger(config, "keyUse", static_cast<int>(mpUseKey->GetSelectedKey()));
	WriteInteger(config, "keyQuickSave", static_cast<int>(mpQuickSaveKey->GetSelectedKey()));
	WriteInteger(config, "keyQuickLoad", static_cast<int>(mpQuickLoadKey->GetSelectedKey()));
	WriteInteger(config, "spotShadowsEnabled", ruEngine::IsSpotLightShadowsEnabled() ? 1 : 0);
	WriteInteger(config, "hdrEnabled", ruEngine::IsHDREnabled() ? 1 : 0);
	WriteInteger(config, "keyStealth", static_cast<int>(mpStealthKey->GetSelectedKey()));
	WriteInteger(config, "textureFiltering", mpTextureFiltering->GetCurrentValue());
	WriteInteger(config, "keyLookLeft", static_cast<int>(mpLookLeftKey->GetSelectedKey()));
	WriteInteger(config, "keyLookRight", static_cast<int>(mpLookRightKey->GetSelectedKey()));
	WriteInteger(config, "showFPS", mpFPSButton->IsChecked() ? 1 : 0);
	WriteInteger(config, "parallax", mpParallaxButton->IsChecked() ? 1 : 0);
	WriteFloat(config, "fov", mFOVSlider->GetValue());
	config.close();
}

void Menu::WriteString(ofstream & stream, string name, string value) {
	stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteInteger(ofstream & stream, string name, int value) {
	stream << name << "=\"" << value << "\";\n";
}

void Menu::WriteFloat(ofstream & stream, string name, float value) {
	stream << name << "=\"" << value << "\";\n";
}

Menu::~Menu() {

}

void Menu::SetMainPageVisible(bool state) {
	mGUIMainButtonsCanvas->SetVisible(state);
}

void Menu::SetAuthorsPageVisible(bool state) {
	mGUIAuthorsBackground->SetVisible(state);
}

void Menu::SetOptionsPageVisible(bool state) {
	mGUIOptionsCanvas->SetVisible(state);
}

Parser * Menu::GetLocalization() {
	return &mLocalization;
}

bool Menu::IsVisible() {
	return mVisible;
}

void Menu::CameraFloating() {
	mCameraAnimationOffset = mCameraAnimationOffset.Lerp(mCameraAnimationNewOffset, 0.00065f);

	if ((mCameraAnimationOffset - mCameraAnimationNewOffset).Length2() < 0.015) {
		mCameraAnimationNewOffset = ruVector3(frandom(-0.25, 0.25), frandom(-0.25, 0.25), frandom(-0.25, 0.25));
	}

	mpCamera->mCamera->SetPosition(mCameraInitialPosition + mCameraAnimationOffset);
}

void Menu::OnHDRButtonClick() {
	ruEngine::SetHDREnabled(mpHDRButton->IsChecked());
}
