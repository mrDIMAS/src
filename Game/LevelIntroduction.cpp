#include "LevelIntroduction.h"




void LevelIntroduction::Hide()
{

}

void LevelIntroduction::Show()
{

}

void LevelIntroduction::DoScenario()
{
  if( intro )
    textAlphaTo = 255.0f;
  else
    textAlphaTo = 0.0f;

  float fadeSpeed = 0.025f;

  if( !intro )
    fadeSpeed = 0.05f;

  textAlpha += ( textAlphaTo - textAlpha ) * 0.025f;

  int scx = GetResolutionWidth() / 2;
  int scy = GetResolutionHeight() / 2;

  int w = 600;
  int h = 400;

  DrawGUIText( text.c_str(), scx - w / 2, scy - h / 2, w, h, gui->font, Vector3( 255, 255, 255 ), 0, textAlpha );

  if( GetElapsedTimeInSeconds( textTimer ) >= 22 || mi::KeyHit( mi::Space ) )
    intro = false;

  if( intro )
    DrawGUIText( "[Space] - пропустить", GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

  if( intro == false )
  {
    if( textAlpha < 5.0f )
      Level::Change( LevelName::L1Arrival );
  }
}

LevelIntroduction::~LevelIntroduction()
{

}

LevelIntroduction::LevelIntroduction()
{
  scene = CreateSceneNode();
  textAlpha = 0.0f;
  textAlphaTo = 255.0f;
  textTimer = CreateTimer( );
  intro = true;
  LoadLocalization( "intro.loc" );
  text = localization.GetString( "intro" );
}
