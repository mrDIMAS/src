#include "WaitKeyButton.h"




void WaitKeyButton::Draw( float x, float y, TextureHandle buttonImage, const char * text )
{
  int textHeight = 16;
  float buttonWidth = 60; 
  float buttonHeight = 32;

  GUIState controlButton = DrawGUIButton( x, y, buttonWidth, buttonHeight, buttonImage, grabKey ? "[ Key ]" : desc.c_str(), gui->font, Vector3( 0, 255, 0 ), 1 );
  DrawGUIText( text, x + buttonWidth * 1.1f, y + textHeight / 2, 100, textHeight, gui->font, Vector3( 0, 255, 0 ), 0 );

  if( controlButton.mouseLeftClicked )
  {
    grabKey = true;
  }

  if( grabKey )
  {
    for( int i = 0; i < 255; i++ )
    {
      if( mi::KeyDown( (mi::Key)i ))
      { 
        SetSelected( i );

        grabKey = false;
      }
    }      
  }
}

void WaitKeyButton::SetSelected( int i )
{
  if( i != mi::LeftShift && i != mi::RightShift && i != mi::Space && i != mi::Tab )
  {
    unsigned char buffer[ 32 ] = { 0 };
    int num = scan2ascii( i, (unsigned short*)buffer );

    desc.clear();

    for( int j = 0; j < num; j++ )
    {            
      desc.push_back( buffer[ j ] );
    }
  }
  else
  {
    if( i == mi::LeftShift )
      desc = "L Shift";
    if( i == mi::RightShift )
      desc = "R Shift";
    if( i == mi::Space )
      desc = "Space";
    if( i == mi::Tab )
      desc = "Tab";
  }

  selectedKey = i;
}

WaitKeyButton::WaitKeyButton()
{
  desc = " ";
  grabKey= false;
}

