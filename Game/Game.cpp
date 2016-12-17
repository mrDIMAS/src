#include "Precompiled.h"
#include "Game.h"
#include "Menu.h"
#include "Player.h"
#include "GUIProperties.h"
#include "Level.h"
#include "LightAnimator.h"
#include "SaveFile.h"
#include "Utils.h"
#include "SoundMaterial.h"
#include "LevelMine.h"
#include "LevelResearchFacility.h"
#include "LevelSewers.h"
#include "LevelCutsceneIntro.h"
#include "LevelForest.h"
#include "LevelEnding.h"
#include "LevelArrival2.h"

// force OS to use high-performance GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

unique_ptr<Game> Game::msInstance;

Game::Game() :
	mKeyQuickSave(IInput::Key::F5),
	mKeyQuickLoad(IInput::Key::F9),
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
				if(mLevel) {
					// quick save
					if(mEngine->GetInput()->IsKeyHit(mKeyQuickSave)) {
						SaveState("quickSave.save");
					}

					// quick load
					if(mEngine->GetInput()->IsKeyHit(mKeyQuickLoad)) {
						if(IsFileExists("quickSave.save")) {
							LoadState("quickSave.save");
						}
					}

					// autosaving 
					if(mAutoSaveTimer->GetElapsedTimeInSeconds() >= 60) {
						if(mLevel->GetPlayer()) {
							const bool enemyFarEnough = mLevel->GetEnemy() ? mLevel->GetEnemy()->GetCurrentPosition().Distance(mLevel->GetPlayer()->GetCurrentPosition()) > 12.0f : true;
							const bool notDetected = mLevel->GetEnemy() ? mLevel->GetEnemy()->GetMoveType() != Enemy::MoveType::ChasePlayer : true;
							if(!mLevel->GetPlayer()->IsDead() && notDetected && enemyFarEnough) {
								SaveState("autosave.save");
								mAutoSaveTimer->Restart();
							}
						}
					}

					// level scenario
					mLevel->GenericUpdate();
					mLevel->DoScenario();

					// delete level if ended, or if player is dead
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

void Game::LoadState(const string & savefileName) {
	SaveFile saveFile(savefileName, false);
	int levNum = 0;
	saveFile & levNum;
	// load level
	LoadLevel((LevelName)levNum);
	// deserialize it's objects
	if(mLevel) {
		mLevel->Serialize(saveFile);
	}
}

void Game::SaveState(const string & savefileName) {
	SaveFile saveFile(savefileName, true);
	if(mLevel) {
		bool visible = mLevel->IsVisible();
		if(!visible) {
			mLevel->Show();
		}
		// oh, those casts...
		int name = (int)mLevel->mName;
		saveFile & name;
		mLevel->Serialize(saveFile);
		if(!visible) {
			mLevel->Hide();
		}
	}
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
	Config cfg("config.cfg");
	string resolution = cfg.GetString("resolution");
	int resW = atoi(resolution.substr(0, resolution.find('x')).c_str());
	int resH = atoi(resolution.substr(resolution.find('x') + 1).c_str());
	bool vsync = cfg.GetBoolean("vsync");
	bool fullscreen = cfg.GetBoolean("windowMode");
	mInitialLevel = (LevelName)((int)cfg.GetNumber("newGameStartLevel"));
	mLocalizationPath = cfg.GetString("languagePath");

	// create engine and set defaults
	mEngine = IEngine::Create(resW, resH, fullscreen, vsync);
	mEngine->GetSceneFactory()->SetPointLightDefaultTexture(mEngine->GetRenderer()->GetCubeTexture("data/textures/generic/pointCube.dds"));
	mEngine->GetSceneFactory()->SetSpotLightDefaultTexture(mEngine->GetRenderer()->GetTexture("data/textures/generic/spotlight.jpg"));

	pGUIProp = make_unique<GUIProperties>(Instance());
	mMenu = make_unique<Menu>(Instance());
	mEngine->GetRenderer()->SetCursor(mEngine->GetRenderer()->GetTexture("data/gui/cursor.tga"), 32, 32);
	mDeltaTimer = ITimer::Create();
	Config loc(mLocalizationPath + "menu.loc");
	mLoadingScreen = make_unique<LoadingScreen>(Instance(), loc.GetString("loading"));
	mOverlayScene = mEngine->CreateGUIScene();
	mFPSText = mOverlayScene->CreateText("FPS", 0, 0, 200, 200, pGUIProp->mFont, pGUIProp->mForeColor, TextAlignment::Left, 100);
	mAutoSaveTimer = ITimer::Create();
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

void Game::SetQuickSaveKey(const IInput::Key & key) {
	mKeyQuickSave = key;
}

void Game::SetQuickLoadKey(const IInput::Key & key) {
	mKeyQuickLoad = key;
}

void Game::SetMusicVolume(float vol) {
	mMusicVolume = vol;
}

float Game::GetMusicVolume() const {
	return mMusicVolume;
}

const unique_ptr<IEngine> & Game::GetEngine() const {
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
		LoadState("lastGame.save");
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
	mGUILoadingText = mScene->CreateText(loadingText, x, y, w, h, mGUIFont, Vector3(0, 0, 0), TextAlignment::Center);
	mGUILoadingText->SetLayer(0xFF); // topmost
	mGUILoadingBackground = mScene->CreateRect(0, 0, ruVirtualScreenWidth, ruVirtualScreenHeight, mGame->GetEngine()->GetRenderer()->GetTexture("data/gui/loadingscreen.tga"), pGUIProp->mBackColor);
}

void LoadingScreen::Draw() {
	mScene->SetVisible(true);
	mGame->GetEngine()->GetRenderer()->SetCursorVisible(false);
	mGame->GetEngine()->GetRenderer()->RenderWorld();
	mScene->SetVisible(false);
}

int main(int argc, char * argv[]) {
	ShowWindow(GetConsoleWindow(), FALSE); // console disabled in release

	if(SteamAPI_Init() == false) {
		return 1;
	}

	try {
		Game::MakeInstance();
		Game::Instance()->Start();
		Game::DestroyInstance();
	} catch(std::exception & err) {
		SteamAPI_Shutdown();
		MessageBoxA(0, err.what(), "Runtime exception", MB_OK | MB_ICONERROR);
		return 1;
	}
	SteamAPI_Shutdown();
	return 0;
}
