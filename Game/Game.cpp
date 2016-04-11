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

double g_dt = 1.0f;
ruInput::Key g_keyQuickSave = ruInput::Key::F5;
ruInput::Key g_keyQuickLoad = ruInput::Key::F9;
string localizationPath;
bool g_showFPS = false;
bool g_running = true;
float mouseSens = 0.5f;
float g_musicVolume = 1.0f;
double gFixedTick = 1.0 / 60.0; // 0.016(6) sec
double gameClock;
shared_ptr<ruTimer> dtTimer;

void Game_UpdateClock() {
	gameClock = dtTimer->GetTimeInSeconds();
}

int main( int argc, char * argv[] ) {
	try {
		Parser config;
		config.ParseFile( "mine.cfg" );

		float resW = config.GetNumber( "resW" );
		float resH = config.GetNumber( "resH" );
		int fullscreen = config.GetNumber( "fullscreen" );
		char vSync = config.GetNumber( "vSync" );
		g_initialLevel = config.GetNumber( "levelNum" );
		g_showFPS = config.GetNumber( "debugInfo" ) != 0.0f;
		localizationPath = config.GetString( "languagePath" );

	#ifdef _DEBUG
		ruEngine::Create( 0, 0, 0, vSync );
	#else
		ruEngine::Create( resW, resH, fullscreen, vSync );
	#endif
		ruPointLight::SetPointDefaultTexture( ruCubeTexture::Request( "data/textures/generic/pointCube.dds" ));
		ruSpotLight::SetSpotDefaultTexture( ruTexture::Request( "data/textures/generic/spotlight.jpg" ));
	
		pGUIProp = new GUIProperties;
		pMainMenu = new Menu;
		ruEngine::SetCursorSettings( ruTexture::Request( "data/gui/cursor.tga" ), 32, 32 );
		FPSCounter fpsCounter;

		dtTimer = ruTimer::Create();
		Level::CreateLoadingScreen();

		Game_UpdateClock();

		shared_ptr<ruText> fpsText = ruText::Create( "FPS", 0, 0, 200, 200, pGUIProp->mFont, pGUIProp->mForeColor, ruTextAlignment::Left, 100 );
		ruEngine::ShowCursor();

		while( g_running ) {
			ruEngine::RenderWorld( );
			 
			double dt = dtTimer->GetTimeInSeconds() - gameClock;
			while( dt >= gFixedTick ) {
				dt -= gFixedTick;
				gameClock += gFixedTick;              
				g_dt = gFixedTick;

				if( !pMainMenu->IsVisible() )  {
					ruPhysics::Update( gFixedTick, 1, gFixedTick );
				}

				ruInput::Update();
				 
				if( pPlayer ) {
					pPlayer->Update();
				}

				pMainMenu->Update();

				if( !pMainMenu->IsVisible() ) {
					ActionTimer::UpdateAll();
					if( ruInput::IsKeyHit( g_keyQuickSave )) {
						SaveWriter( "quickSave.save" ).SaveWorldState();
						pPlayer->SetTip( config.GetString( "saved" ) );
					}
					if( ruInput::IsKeyHit( g_keyQuickLoad )) {
						if( IsFileExists( "quickSave.save" )) {
							SaveLoader( "quickSave.save" ).RestoreWorldState();
							pPlayer->SetTip( config.GetString( "loaded" ) );
							Game_UpdateClock();
						}
					}
					if( pCurrentLevel ) {
						pCurrentLevel->UpdateGenericObjectsIdle();
						pCurrentLevel->DoScenario();
					}
					InteractiveObject::UpdateAll();
				}
			
				fpsText->SetText( StringBuilder( "DIPs: " ) << ruEngine::GetDIPs() << "\nTCs: " << ruEngine::GetTextureUsedPerFrame() << "\nFPS: " << fpsCounter.fps );
				fpsText->SetVisible( g_showFPS );
				// recalculate transforms of scene nodes
				ruEngine::UpdateWorld();
			}			
			fpsCounter.RegisterFrame();
		}

		dtTimer.reset();

		if( pCurrentLevel ) {
			delete pCurrentLevel;
		}
		if( pPlayer ) {
			delete pPlayer;
		}
		delete pMainMenu;
		delete pGUIProp;
		ruEngine::Free();
	} catch( std::exception & err ) {
		MessageBoxA( 0, err.what(), "Exception caugth!", MB_OK | MB_ICONERROR );
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
