#include "Game.h"
#include "Menu.h"
#include "Player.h"
#include "GUI.h"
#include "Level.h"
#include "LightAnimator.h"
#include "FPSCounter.h"
#include "SaveWriter.h"
#include "SaveLoader.h"
#include "Utils.h"

float g_resW;
float g_resH;
double g_dt = 1.0f;
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

    g_resW = config.GetNumber( "resW" );
    g_resH = config.GetNumber( "resH" );
    int fullscreen = config.GetNumber( "fullscreen" );
    char vSync = config.GetNumber( "vSync" );
    g_initialLevel = config.GetNumber( "levelNum" );
    g_showFPS = config.GetNumber( "debugInfo" ) != 0.0f;
    localizationPath = config.GetString( "languagePath" );

#ifdef _DEBUG
    ruCreateRenderer( 0, 0, 0, vSync );
#else
    ruCreateRenderer( g_resW, g_resH, fullscreen, vSync );
#endif
	// get actual resolution settings
	g_resW = ruGetResolutionWidth();
	g_resH = ruGetResolutionHeight();

    ruSetLightPointDefaultTexture( ruGetCubeTexture( "data/textures/generic/pointCube.dds" ));
    ruSetLightSpotDefaultTexture( ruGetTexture( "data/textures/generic/spotlight.jpg" ));
	
    pGUI = new GUI;
    pMainMenu = new Menu;
    ruSetCursorSettings( ruGetTexture( "data/gui/cursor.png" ), 32, 32 );
    FPSCounter fpsCounter;

    ruTimerHandle dtTimer = ruCreateTimer();
	Level::CreateLoadingScreen();

    double fixedTick = 1.0 / 60.0;
    double gameClock = ruGetTimeInSeconds( dtTimer );

    ruTextHandle fpsText = ruCreateGUIText( "FPS", 0, 0, 200, 200, pGUI->mFont, ruVector3( 255, 0, 255 ), 0, 100 );
    ruShowCursor();

    while( g_running ) {
        try {
            ruRenderWorld( fixedTick );

            double dt = ruGetTimeInSeconds( dtTimer ) - gameClock;
            while( dt >= fixedTick ) {
                dt -= fixedTick;
                gameClock += fixedTick;              
				g_dt = fixedTick;

				if( !pMainMenu->IsVisible() ) {
					ruUpdatePhysics( fixedTick, 1, fixedTick );
				}

                ruInputUpdate();

                if( pPlayer ) {
                    pPlayer->Update();
                }

                pMainMenu->Update();

                if( !pMainMenu->IsVisible() ) {
                    if( ruIsKeyHit( g_keyQuickSave )) {
                        SaveWriter( "quickSave.save" ).SaveWorldState();
                        pPlayer->SetTip( config.GetString( "saved" ) );
                    }
                    if( ruIsKeyHit( g_keyQuickLoad )) {
                        if( FileExist( "quickSave.save" )) {
                            SaveLoader( "quickSave.save" ).RestoreWorldState();
                            pPlayer->SetTip( config.GetString( "loaded" ) );
                        }
                    }
                    if( pCurrentLevel ) {
                        pCurrentLevel->DoScenario();
                    }
                    InteractiveObject::UpdateAll();
				}
                ruSetGUINodeText( fpsText, Format( "DIPs: %d\nTCs: %d\nFPS: %d", ruDIPs(), ruTextureUsedPerFrame(), fpsCounter.fps ).c_str());
                ruSetGUINodeVisible( fpsText, g_showFPS );
				// recalculate transforms of scene nodes
				ruUpdateWorld();
            }			
			fpsCounter.RegisterFrame();
        } catch( runtime_error ) {
            break;
        }
    }

    if( pCurrentLevel ) {
        delete pCurrentLevel;
    }
    if( pPlayer ) {
        delete pPlayer;
    }
    delete pMainMenu;
    delete pGUI;
    ruFreeRenderer();
}

std::string Format( const char * format, ... ) {
    char buffer[ 1024 ];
    va_list	argumentList;

    va_start( argumentList, format );
    vsprintf_s( buffer, format , argumentList);
    va_end(argumentList);

    return string( buffer );
}

float frandom( float low, float high ) {
    return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
}

void RaiseError( const string & text ) {
    MessageBoxA( 0, text.c_str(), "CriticalError", MB_OK | MB_ICONERROR );
    throw runtime_error( text.c_str() );
}
