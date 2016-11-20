#include "Precompiled.h"
#include "Game.h"
#include "Menu.h"
#include "Player.h"
#include "GUIProperties.h"
#include "Level.h"
#include "LightAnimator.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"
#include "SoundMaterial.h"
#include "LevelArrival.h"
#include "LevelMine.h"
#include "LevelResearchFacility.h"
#include "LevelSewers.h"
#include "LevelCutsceneIntro.h"
#include "LevelForest.h"
#include "LevelEnding.h"

// force OS to use high-performance GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

unique_ptr<Game> Game::msInstance;

Game::Game() :
	mKeyQuickSave(ruInput::Key::F5),
	mKeyQuickLoad(ruInput::Key::F9),
	mShowFPS(true),
	mRunning(true),
	mMouseSens(0.5f),
	mMusicVolume(1.0f) {

}

void Game::UpdateClock() {
	mGameClock = mDeltaTimer->GetTimeInSeconds();
}

void Game::MainLoop() {
	double gFixedTick = 1.0 / 60.0;
	while(mRunning) {
		mEngine->GetRenderer()->RenderWorld();

		double dt = mDeltaTimer->GetTimeInSeconds() - mGameClock;
		while(dt >= gFixedTick) {
			dt -= gFixedTick;
			mGameClock += gFixedTick;

			if(!mMenu->IsVisible()) {
				mEngine->GetPhysics()->Update(gFixedTick, 1, gFixedTick);
			}

			mEngine->GetInput()->Update();

			mMenu->Update();

			if(!mMenu->IsVisible()) {
				if(mEngine->GetInput()->IsKeyHit(mKeyQuickSave)) {
					SaveWriter("quickSave.save").SaveWorldState();
				}
				if(mEngine->GetInput()->IsKeyHit(mKeyQuickLoad)) {
					if(IsFileExists("quickSave.save")) {
						SaveLoader("quickSave.save").RestoreWorldState();
					}
				}
				if(mLevel) {
					mLevel->GenericUpdate();
					mLevel->DoScenario();

					if(mLevel->mEnded) {
						mLevel.reset();
					} else {
						if(mLevel->GetPlayer()) {
							if(mLevel->GetPlayer()->IsDead()) {
								mLevel.reset();
							}
						}
					}
				}
			}

			mFPSText->SetText(
				StringBuilder("DIPs: ") << mEngine->GetRenderer()->GetDIPs() <<
				"\nTCC: " << mEngine->GetRenderer()->GetTextureUsedPerFrame() <<
				"\nFPS: " << mFPSCounter.fps <<
				"\nSCC: " << mEngine->GetRenderer()->GetShaderUsedPerFrame() <<
				"\nRTC: " << mEngine->GetRenderer()->GetRenderedTriangles()
			);

			mFPSText->SetVisible(mShowFPS);
			// recalculate transforms of scene nodes
			mEngine->GetRenderer()->UpdateWorld();
		}
		mFPSCounter.RegisterFrame();
	}
}

void Game::Shutdown() {
	mRunning = false;
}

string Game::GetLocalizationPath() const {
	return mLocalizationPath;
}

Game::~Game() {
	mLoadingScreen.reset();
	mLevel.reset();
	mMenu.reset();
	mEngine.reset();
}

void Game::Start() {
	// read config
	ruConfig cfg("config.cfg");
	string resolution = cfg.GetString("resolution");
	int resW = atoi(resolution.substr(0, resolution.find('x')).c_str());
	int resH = atoi(resolution.substr(resolution.find('x') + 1).c_str());
	bool vsync = cfg.GetBoolean("vsync");
	bool fullscreen = cfg.GetBoolean("windowMode");
	mInitialLevel = (LevelName)((int)cfg.GetNumber("newGameStartLevel"));
	mLocalizationPath = cfg.GetString("languagePath");

	// create engine and set defaults
	mEngine = ruEngine::Create(resW, resH, fullscreen, vsync);
	ruPointLight::SetPointDefaultTexture(ruCubeTexture::Request("data/textures/generic/pointCube.dds"));
	ruSpotLight::SetSpotDefaultTexture(ruTexture::Request("data/textures/generic/spotlight.jpg"));

	pGUIProp = make_unique<GUIProperties>(Instance());
	mMenu = make_unique<Menu>(Instance());
	mEngine->GetRenderer()->SetCursor(ruTexture::Request("data/gui/cursor.tga"), 32, 32);
	mDeltaTimer = ruTimer::Create();
	ruConfig loc(mLocalizationPath + "menu.loc");
	mLoadingScreen = unique_ptr<LoadingScreen>(new LoadingScreen(Instance(), loc.GetString("loading")));
	mOverlayScene = mEngine->CreateGUIScene();
	mFPSText = mOverlayScene->CreateText("FPS", 0, 0, 200, 200, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left, 100);
	mEngine->GetRenderer()->SetCursorVisible(true);
	UpdateClock();
	MainLoop();
}

void Game::SetMouseSensitivity(float sens) {
	mMouseSens = sens;
	if(mMouseSens < 0.05) {
		mMouseSens = 0.05;
	}
}

float Game::GetMouseSensitivity() const {
	return mMouseSens;
}

LevelName Game::GetNewGameLevel() const {
	return mInitialLevel;
}

void Game::ShowFPS(bool state) {
	mShowFPS = state;
}

bool Game::IsShowFPSEnabled() const {
	return mShowFPS;
}

void Game::SetQuickSaveKey(const ruInput::Key & key) {
	mKeyQuickSave = key;
}

void Game::SetQuickLoadKey(const ruInput::Key & key) {
	mKeyQuickLoad = key;
}

void Game::SetMusicVolume(float vol) // REPLACE THIS OR REMOVE
{
	mMusicVolume = vol;
}

float Game::GetMusicVolume() const// REPLACE THIS OR REMOVE
{
	return mMusicVolume;
}

const unique_ptr<ruEngine> & Game::GetEngine() const {
	return mEngine;
}

const unique_ptr<Level>& Game::GetLevel() const {
	return mLevel;
}

const unique_ptr<Menu> & Game::GetMenu() const {
	return mMenu;
}

void Game::LoadLevel(LevelName name, bool continueFromSave) {
	static LevelName lastLevel = LevelName::Undefined;

	Level::msCurLevelID = name;

	// save player state - grim stuff
	unique_ptr<PlayerTransfer> playerTransfer;

	if(mLevel) {
		if(mLevel->GetPlayer()) {
			playerTransfer = make_unique<PlayerTransfer>();
			mLevel->GetPlayer()->GetInventory()->GetItems(playerTransfer->mItems);
			playerTransfer->mHealth = mLevel->GetPlayer()->GetHealth();
		}

		// delete player
		mLevel->DestroyPlayer();
	}

	// delete level
	mLevel.reset();

	mLoadingScreen->Draw();

	// load new level and restore player state
	switch(Level::msCurLevelID) {
	case LevelName::Intro: mLevel = make_unique<LevelIntro>(Instance(), playerTransfer); break;
	case LevelName::Arrival: mLevel = make_unique<LevelArrival>(Instance(), playerTransfer); break;
	case LevelName::Mine: mLevel = make_unique<LevelMine>(Instance(), playerTransfer); break;
	case LevelName::ResearchFacility: mLevel = make_unique<LevelResearchFacility>(Instance(), playerTransfer); break;
	case LevelName::Sewers: mLevel = make_unique<LevelSewers>(Instance(), playerTransfer); break;
	case LevelName::Forest: mLevel = make_unique<LevelForest>(Instance(), playerTransfer); break;
	case LevelName::Ending: mLevel = make_unique<LevelEnding>(Instance(), playerTransfer); break;
	default: throw runtime_error("Unable to load level with bad id!");
	}

	if(continueFromSave) {
		SaveLoader("lastGame.save").RestoreWorldState();
	}

	if(mLevel->GetPlayer()) {
		mLevel->GetPlayer()->GetHUD()->SetTip(mLevel->GetPlayer()->GetLocalization()->GetString("loaded"));
	}

	// fill sound list for react to
	if(mLevel->GetEnemy()) {
		for(auto & pMat : mLevel->GetPlayer()->mSoundMaterialList) {
			for(auto & pSound : pMat->GetSoundList()) {
				mLevel->GetEnemy()->mReactSounds.push_back(pSound);
			}
		}
		mLevel->GetEnemy()->mReactSounds.insert(mLevel->GetEnemy()->mReactSounds.end(), mLevel->GetPlayer()->mPainSound.begin(), mLevel->GetPlayer()->mPainSound.end());
		mLevel->GetEnemy()->mReactSounds.push_back(mLevel->GetPlayer()->mFlashlightSwitchSound);
	}

	lastLevel = name;

	UpdateClock();
}

void Game::MakeInstance() {
	msInstance = make_unique<Game>();
}

unique_ptr<Game>& Game::Instance() {
	if(!msInstance) {
		throw runtime_error("Game is not instantiated!");
	}
	return msInstance;
}

void Game::DestroyInstance() {
	msInstance.reset();
}

LoadingScreen::LoadingScreen(unique_ptr<Game> & game, const string & loadingText) : mGame(game) {
	int w = 200;
	int h = 32;
	int x = (ruVirtualScreenWidth - w) / 2;
	int y = (ruVirtualScreenHeight - h) / 2;
	mScene = mGame->GetEngine()->CreateGUIScene();
	mScene->SetVisible(false);
	mGUIFont = mGame->GetEngine()->CreateBitmapFont(32, "data/fonts/font5.ttf");
	mGUILoadingText = mScene->CreateText(loadingText, x, y, w, h, mGUIFont, ruVector3(0, 0, 0), ruTextAlignment::Center);
	mGUILoadingText->SetLayer(0xFF); // topmost
	mGUILoadingBackground = mScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, ruTexture::Request("data/gui/loadingscreen.tga"), pGUIProp->mBackColor);
}

void LoadingScreen::Draw() {
	mScene->SetVisible(true);
	mGame->GetEngine()->GetRenderer()->SetCursorVisible(false);
	mGame->GetEngine()->GetRenderer()->RenderWorld();
	mScene->SetVisible(false);
}

int main(int argc, char * argv[]) {
	try {
		Game::MakeInstance();
		Game::Instance()->Start();
		Game::DestroyInstance();
	} catch(std::exception & err) {
		MessageBoxA(0, err.what(), "Runtime exception", MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}
