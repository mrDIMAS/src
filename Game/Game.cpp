#include "Game.h"
#include "Menu.h"
#include "Player.h"
#include "GUI.h"
#include "Level.h"
#include "LightAnimator.h"
#include "ScreenScreamer.h"
#include "FPSCounter.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"

float g_resW;
float g_resH;
float g_dt = 1.0f;
int g_keyQuickSave = KEY_F5;
int g_keyQuickLoad = KEY_F9;
string localizationPath;
bool g_showFPS = false;
bool g_running = true;
float mouseSens = 0.5f;
float g_musicVolume = 1.0f;


void main( )
{
    Parser config;
    config.ParseFile( "mine.cfg" );

    g_resW            = config.GetNumber( "resW" );
    g_resH            = config.GetNumber( "resH" );
    int fullscreen    = config.GetNumber( "fullscreen" );
    char vSync		  = config.GetNumber( "vSync" );
    g_initialLevel    = config.GetNumber( "levelNum" );
    g_showFPS         = config.GetNumber( "debugInfo" ) != 0.0f;
    localizationPath  = config.GetString( "languagePath" );

#ifdef _DEBUG
    ruCreateRenderer( 0, 0, 0, vSync );
#else
    ruCreateRenderer( g_resW, g_resH, fullscreen, vSync );
#endif

    ruSetLightPointDefaultTexture( ruGetCubeTexture( "data/textures/generic/pointCube.dds" ));
    ruSetLightSpotDefaultTexture( ruGetTexture( "data/textures/generic/spotlight.jpg" ));
    g_resW = ruGetResolutionWidth();
    g_resH = ruGetResolutionHeight();

    pGUI = new GUI;
    pMainMenu = new Menu;
    int escHit = 0;
    screamer = new ScreenScreamer;
    ruSetCursorSettings( ruGetTexture( "data/gui/cursor.png" ), 32, 32 );
    FPSCounter fpsCounter;

    ruTimerHandle dtTimer = ruCreateTimer();


	/*
    g_dt = 1.0f  / 60.0f;
    float lastTime = 0.0f;
    //float dtDest = g_dt;
    float minDt = 100000;
	float accumDeltaTime = 0.0f;
	float fixedTimeStep = 1.0f / 60.0f;*//*
	float physicsStep = 1.0f / 120.0f;
	float maxPhysicsTimePerFrame = 0.15f;
	float lastTime = ruGetTimeInSeconds( dtTimer );
	float physicsTimer = 0;*/

	double fixedTick = 1.0 / 60.0;
	double prevPhysTime = ruGetTimeInSeconds( dtTimer );
	double currPhysTime = prevPhysTime;
	double gameClock = prevPhysTime;

	ruTextHandle fpsText = ruCreateGUIText( "FPS", 0, 0, 200, 200, pGUI->mFont, ruVector3( 255, 0, 255 ), 0, 100 );
    while( true )
    {
		try {			
			if( !g_running )
				break;

			// ===========================
			// frame rendering update
			ruRenderWorld( 1.0f / 60.0f );
			
			// ===========================
			// physics update
			if( !pMainMenu->IsVisible() )
			{
				currPhysTime = ruGetTimeInSeconds( dtTimer );
				ruUpdatePhysics( currPhysTime - prevPhysTime, 16, 1.0f / 60.0f );
				prevPhysTime = currPhysTime;
			}

			// ===========================
			// game logics update
			double dt = ruGetTimeInSeconds( dtTimer ) - gameClock;
			if( dt > 1.0f / 10.0f )
				dt = 1.0f / 10.0f;
			while( dt >= fixedTick ) 
			{
				dt -= fixedTick;
				gameClock += fixedTick;

				g_dt = fixedTick;

				ruInputUpdate();
				if( pPlayer ) 
				{
					pPlayer->Update();
				}

				pMainMenu->Update();			

				if( !pMainMenu->IsVisible() )
				{
					if( ruIsKeyHit( g_keyQuickSave ))
					{
						SaveWriter( "quickSave.save" ).SaveWorldState();
						pPlayer->SetTip( config.GetString( "saved" ) );
					}
					if( ruIsKeyHit( g_keyQuickLoad ))
					{
						if( FileExist( "quickSave.save" ))
						{
							SaveLoader( "quickSave.save" ).RestoreWorldState();
							pPlayer->SetTip( config.GetString( "loaded" ) );
						}
						if( pCurrentLevel )
						{
							pCurrentLevel->DoScenario();
						}
					}
					InteractiveObject::UpdateAll();
					screamer->Update();
				}			
				ruSetGUINodeText( fpsText, Format( "DIPs: %d\nTCs: %d\nFPS: %d\ndt: %f\n", ruDIPs(), ruTextureUsedPerFrame(), fpsCounter.fps, g_dt ).c_str());
				ruSetGUINodeVisible( fpsText, g_showFPS );
			}		
			fpsCounter.RegisterFrame();
		} catch( runtime_error & rError ) {
			// just exit main loop to be sure, that all resources will be freed
			break;
		}
    }

    if( pCurrentLevel )
        delete pCurrentLevel;
    if( pPlayer )
        delete pPlayer;
    delete screamer;
    delete pMainMenu;
    delete pGUI;
    ruFreeRenderer();
}
