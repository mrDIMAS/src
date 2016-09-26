#include "Precompiled.h"

#include "Game.h"
#include "Menu.h"
#include "Player.h"
#include "GUIProperties.h"
#include "Level.h"
#include "LightAnimator.h"
#include "FPSCounter.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"
#include "SoundMaterial.h"

ruInput::Key gKeyQuickSave = ruInput::Key::F5;
ruInput::Key gKeyQuickLoad = ruInput::Key::F9;
string gLocalizationPath;
bool gShowFPS = false;
bool gRunning = true;
float gMouseSens = 0.5f;
float gMusicVolume = 1.0f;
double gFixedTick = 1.0 / 60.0; // 0.016(6) sec
double gGameClock;
shared_ptr<ruTimer> gDeltaTimer;

void Game_UpdateClock() {
	gGameClock = gDeltaTimer->GetTimeInSeconds();
}

int main(int argc, char * argv[]) {
	try {
		Parser config;
		config.ParseFile("mine.cfg");

		float resW = config.GetNumber("resW");
		float resH = config.GetNumber("resH");
		int fullscreen = config.GetNumber("fullscreen");
		char vSync = config.GetNumber("vSync");
		g_initialLevel = config.GetNumber("levelNum");
		gShowFPS = config.GetNumber("debugInfo") != 0.0f;
		gLocalizationPath = config.GetString("languagePath");

#ifdef _DEBUG
		ruEngine::Create(0, 0, 0, vSync);
#else
		ruEngine::Create(resW, resH, fullscreen, vSync);
#endif
		ruPointLight::SetPointDefaultTexture(ruCubeTexture::Request("data/textures/generic/pointCube.dds"));
		ruSpotLight::SetSpotDefaultTexture(ruTexture::Request("data/textures/generic/spotlight.jpg"));

		pGUIProp = make_unique<GUIProperties>();
		pMainMenu = make_unique<Menu>();
		ruEngine::SetCursorSettings(ruTexture::Request("data/gui/cursor.tga"), 32, 32);
		FPSCounter fpsCounter;

		gDeltaTimer = ruTimer::Create();
		Level::CreateLoadingScreen();

		Game_UpdateClock();

		shared_ptr<ruGUIScene> overlapScene = ruGUIScene::Create();
		shared_ptr<ruText> fpsText = overlapScene->CreateText("FPS", 0, 0, 200, 200, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left, 100);
		ruEngine::ShowCursor();

		while (gRunning) {
			ruEngine::RenderWorld();

			double dt = gDeltaTimer->GetTimeInSeconds() - gGameClock;
			while (dt >= gFixedTick) {
				dt -= gFixedTick;
				gGameClock += gFixedTick;

				if (!pMainMenu->IsVisible()) {
					ruPhysics::Update(gFixedTick, 1, gFixedTick);
				}


				ruInput::Update();

				pMainMenu->Update();

				if (!pMainMenu->IsVisible()) {
					ActionTimer::UpdateAll();
					if (ruInput::IsKeyHit(gKeyQuickSave)) {
						SaveWriter("quickSave.save").SaveWorldState();

					}
					if (ruInput::IsKeyHit(gKeyQuickLoad)) {
						if (IsFileExists("quickSave.save")) {
							SaveLoader("quickSave.save").RestoreWorldState();

							Game_UpdateClock();
						}
					}
					if (Level::Current()) {
						Level::Current()->UpdateGenericObjectsIdle();
						Level::Current()->DoScenario();
					}
				}

				fpsText->SetText(
					StringBuilder("DIPs: ") << ruEngine::GetDIPs() <<
					"\nTCC: " << ruEngine::GetTextureUsedPerFrame() <<
					"\nFPS: " << fpsCounter.fps <<
					"\nSCC: " << ruEngine::GetShaderCountChangedPerFrame() <<
					"\nRTC: " << ruEngine::GetRenderedTriangles()
				);

				fpsText->SetVisible(gShowFPS);
				// recalculate transforms of scene nodes
				ruEngine::UpdateWorld();
			}
			fpsCounter.RegisterFrame();
		}

		gDeltaTimer.reset();

		// delete level with stuff (player, objects, etc)
		Level::Purge();
		// delete menu
		pMainMenu.reset();
		// finally delete engine
		ruEngine::Free();

	} catch (std::exception & err) {
		// something went wrong
		MessageBoxA(0, err.what(), "Exception caugth!", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// everything is ok
	return EXIT_SUCCESS;
}
