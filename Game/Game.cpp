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

int g_keyQuickSave = mi::Key::F6;
int g_keyQuickLoad = mi::Key::F9;

string localizationPath;
bool g_showFPS = false;

bool g_running = true;

float mouseSens = 0.5f;

void ParseFile( string fn, map<string,string> & values)
{
  FILE * file = 0;

  fopen_s ( &file, fn.c_str(), "r" );

  if ( !file )
    return;

  string str;

  while ( !feof ( file ) )
  {
    char symbol = '\0';
    fread ( &symbol, sizeof ( char ), 1, file );
    str.push_back ( symbol );
  };

  fclose ( file );

  if ( str.size() <= 1 )
    return;

  values.clear();

  bool equalFound = false;
  bool quoteLF = false, quoteRF = false;
  unsigned int n = 0;

  string varName, var;

  while ( true )
  {
    char symbol = str.at ( n );

    if ( symbol == ';' )
    {
      quoteLF    = false;
      quoteRF    = false;
      equalFound  = false;

      varName.clear();
      var.clear();
    };

    if ( isalpha ( ( unsigned char ) symbol ) || isdigit ( ( unsigned char ) symbol ) || symbol == '_' )
    {
      if ( !equalFound )
        varName.push_back ( symbol );
    }
    else
    {
      if ( symbol == '=' )
        equalFound = true;

      if ( symbol == '"' )
      {
        if ( quoteLF == false )
          quoteLF = true;
        else
          quoteRF = true;
      }
    };

    if ( quoteLF )
    {
      if ( quoteRF )
        values[ varName ] = var;
      else
        if ( symbol != '"' )
          var.push_back ( symbol );
    };

    n++;

    if ( n >= str.size() )
      break;
  };
}

NodeHandle CreateFog( NodeHandle obj, int density )
{
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


void main( )
{
  map<string,string> values;
  ParseFile( "mine.cfg", values );

  int width = atoi( values["resW"].c_str() );
  int height = atoi( values["resH"].c_str() );
  int fullscreen = atoi( values["fullscreen"].c_str() );
  g_initialLevel = (LevelName)atoi( values["levelNum"].c_str() );
  g_showFPS = atoi( values["debugInfo"].c_str() );
  localizationPath = values["languagePath"];
#ifdef _DEBUG
  CreateRenderer( 1366, 768, 0 );
#else
  CreateRenderer( width, height, fullscreen );
#endif

  gui = new GUI;

  menu = new Menu;

  TextureHandle buttonTex = GetTexture( "data/gui/button.png" );

  int escHit = 0;

  screamer = new ScreenScreamer;

  SetCursorSettings( GetTexture( "data/gui/cursor.png" ), 32, 32 );
  
  FPSCounter fpsCounter;

  while( true )
  { 
    if( !g_running )
      break;

    mi::Update();

    if( player )
      player->Update();

    menu->Update();       

    InteractiveObject::UpdateAll();

    if( currentLevel )
      currentLevel->DoScenario();

    fpsCounter.RegisterFrame();
    
    if( g_showFPS )
      DrawGUIText( Format( "DIPs: %d\nTC: %d\nFPS: %d", DIPs(), TextureUsedPerFrame(), fpsCounter.fps ).c_str(), 0, 0, 100, 100, gui->font, Vector3( 255, 0, 255 ), 0, 100 );

    screamer->Update();

    RenderWorld( );

    if( !menu->visible )
    {
      if( mi::KeyHit( (mi::Key)g_keyQuickSave ))
        SaveWriter( "quickSave.save" ).SaveWorldState();

      if( mi::KeyHit( (mi::Key)g_keyQuickLoad ))
        SaveLoader( "quickSave.save" ).RestoreWorldState();
    }
  }

  if( currentLevel )
    delete currentLevel;

  if( player )
    delete player;

  delete screamer;
  delete menu;
  delete gui;

  FreeRenderer();
}
