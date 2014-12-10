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
    ruCreateRenderer( 0, 0, 0, 0 );
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



    g_dt = 1.0f  / 60.0f;
    float lastTime = 0.0f;
    //float dtDest = g_dt;
    float minDt = 100000;
    while( true )
    {
		try {
			if( ruIsKeyHit( KEY_1 ))
				ruSetRenderQuality( 0 );
			if( ruIsKeyHit( KEY_2 ))
				ruSetRenderQuality( 1 );
			ruRestartTimer( dtTimer );
			if( !g_running )
				break;
			ruInputUpdate();
			if( pPlayer )
				pPlayer->Update();
			pMainMenu->Update();
			InteractiveObject::UpdateAll();
			if( !pMainMenu->mVisible )
			{
				if( ruIsKeyHit( g_keyQuickSave ))
				{
					SaveWriter( "quickSave.save" ).SaveWorldState();
					pPlayer->mTip.SetNewText( config.GetString( "saved" ) );
				}
				if( ruIsKeyHit( g_keyQuickLoad ))
					if( FileExist( "quickSave.save" ))
					{
						SaveLoader( "quickSave.save" ).RestoreWorldState();
						pPlayer->mTip.SetNewText( config.GetString( "loaded" ) );
					}
				if( pCurrentLevel )
					pCurrentLevel->DoScenario();
			}
			fpsCounter.RegisterFrame();

			screamer->Update();

			if( g_showFPS )
				ruDrawGUIText( Format( "DIPs: %d\nTCs: %d\nFPS: %d\ndt: %f\nmin dt: %f", ruDIPs(), ruTextureUsedPerFrame(), fpsCounter.fps, g_dt, minDt ).c_str(), 0, 0, 200, 200, pGUI->mFont, ruVector3( 255, 0, 255 ), 0, 100 );
			//if( g_dt < 1.0f / 60.0f )
			ruRenderWorld( g_dt );
			if( g_dt < minDt )
				minDt = g_dt;
			//else
			//	RenderWorld( 1.0f / 60.0f );
			float time = ruGetElapsedTimeInSeconds( dtTimer );
			g_dt = time;
			// lock fps drop on 20fps
			if( g_dt > 1.0f / 20.0f )
				g_dt = 1.0f / 20.0f;
			lastTime = time; 
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
