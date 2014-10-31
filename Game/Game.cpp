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
int g_keyQuickSave = mi::Key::F5;
int g_keyQuickLoad = mi::Key::F9;
string localizationPath;
bool g_showFPS = false;
bool g_running = true;
float mouseSens = 0.5f;

NodeHandle CreateFog( NodeHandle obj, int density ) {
    Vector3 min = GetAABBMin( obj );
    Vector3 max = GetAABBMax( obj );
    NodeHandle particleSystem = CreateParticleSystem( density, GetTexture( "data/textures/particles/p1.png" ) );
    SetParticleSystemSpeedDeviation( particleSystem, Vector3( -0.001, 0.0, -0.001 ),  Vector3(  0.001, 0.0009,  0.001 ) );
    SetParticleSystemBox( particleSystem, min, max );
    SetParticleSystemColors( particleSystem, Vector3( 255, 255, 255 ),  Vector3( 255, 255, 255 ) );
    SetParticleSystemPointSize( particleSystem, 0.5 );
    EnableParticleSystemLighting( particleSystem );
    SetParticleSystemThickness( particleSystem, 0.5 );
    Attach( particleSystem, obj );
    return particleSystem;
}

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

    EnablePointLightShadows( false );
    TimerHandle dtTimer = CreateTimer();
    while( true ) {
        RestartTimer( dtTimer );
        if( !g_running ) {
            break;
        }
        mi::Update();
        if( player ) {
            player->Update();
        }
        menu->Update();
        InteractiveObject::UpdateAll();
        if( !menu->visible ) {
            if( mi::KeyHit( (mi::Key)g_keyQuickSave )) {
                SaveWriter( "quickSave.save" ).SaveWorldState();
                player->tip.SetNewText( config.GetString( "saved" ) );
            }
            if( mi::KeyHit( (mi::Key)g_keyQuickLoad ))
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
            DrawGUIText( Format( "DIPs: %d\nTC: %d\nFPS: %d", DIPs(), TextureUsedPerFrame(), fpsCounter.fps ).c_str(), 0, 0, 100, 100, gui->font, Vector3( 255, 0, 255 ), 0, 100 );
        }
        screamer->Update();
        RenderWorld( );
        g_dt = GetElapsedTimeInMilliSeconds( dtTimer ) / 1000.0f;
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
