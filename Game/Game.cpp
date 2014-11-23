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

void main( ) {
    Parser config;
    config.ParseFile( "mine.cfg" );

    g_resW            = config.GetNumber( "resW" );
    g_resH            = config.GetNumber( "resH" );
    int fullscreen    = config.GetNumber( "fullscreen" );
    g_initialLevel    = config.GetNumber( "levelNum" );
    g_showFPS         = config.GetNumber( "debugInfo" );
    localizationPath  = config.GetString( "languagePath" );

#ifdef _DEBUG
    CreateRenderer( 1366, 768, 0 );
#else
    CreateRenderer( g_resW, g_resH, fullscreen );
#endif

    SetPointDefaultTexture( GetCubeTexture( "data/textures/generic/pointCube.dds" ));
    SetSpotDefaultTexture( GetTexture( "data/textures/generic/spotlight.jpg" ));
    g_resW = GetResolutionWidth();
    g_resH = GetResolutionHeight();

    gui = new GUI;
    menu = new Menu;
    int escHit = 0;
    screamer = new ScreenScreamer;
    SetCursorSettings( GetTexture( "data/gui/cursor.png" ), 32, 32 );
    FPSCounter fpsCounter;

    TimerHandle dtTimer = CreateTimer();



	g_dt = 1.0f  / 60.0f;
	float dtDest = g_dt;
    while( true ) {
        RestartTimer( dtTimer );
        if( !g_running ) {
            break;
        }
        InputUpdate();
        if( player ) {
            player->Update();
        }
        menu->Update();
        InteractiveObject::UpdateAll();
        if( !menu->visible ) {
            if( IsKeyHit( g_keyQuickSave )) {
                SaveWriter( "quickSave.save" ).SaveWorldState();
                player->tip.SetNewText( config.GetString( "saved" ) );
            }
            if( IsKeyHit( g_keyQuickLoad ))
                if( FileExist( "quickSave.save" )) {                    
                    SaveLoader( "quickSave.save" ).RestoreWorldState();
                    player->tip.SetNewText( config.GetString( "loaded" ) );
                }				
            if( currentLevel ) {
                currentLevel->DoScenario();
            }
        }
        fpsCounter.RegisterFrame();
        if( g_showFPS ) {
            DrawGUIText( Format( "DIPs: %d\nTCs: %d\nFPS: %d", DIPs(), TextureUsedPerFrame(), fpsCounter.fps ).c_str(), 0, 0, 200, 200, gui->font, Vector3( 255, 0, 255 ), 0, 100 );
        }
        screamer->Update();
		//g_dt += ( dtDest - g_dt ) * 0.1f;
		// lock dt, ffs it's annoying to debug this shit
		g_dt = 1.0f / 60.0f;
        RenderWorld( g_dt );

        dtDest = GetElapsedTimeInSeconds( dtTimer );
		// lock fps drop on 20fps
		if( dtDest > 1.0f / 20.0f )
			dtDest = 1.0f / 20.0f;
    }

    if( currentLevel ) {
        delete currentLevel;
    }
    if( player ) {
        delete player;
    }
    delete screamer;
    delete menu;
    delete gui;
    FreeRenderer();
}
