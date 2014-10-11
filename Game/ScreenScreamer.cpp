#include "ScreenScreamer.h"
#include "Menu.h"

ScreenScreamer * screamer = 0;

ScreenScreamer::ScreenScreamer()
{
  tex = GetTexture( "data/textures/effects/noise.jpg" );
  sound = CreateSound2D( "data/sounds/screamer.ogg" );

  periods = 0;
  nextPeriodFrames = 0;
  pauseFrames = 0;
}

void ScreenScreamer::DoPeriods( int p )
{
  periods = p;
  nextPeriodFrames = 5 + rand() % 10;
  pauseFrames = 0;
}

void ScreenScreamer::Update()
{
  static int offset = 50;

  if( menu->visible )
  {
    PauseSoundSource( sound );
    return;
  }

  if( periods > 0 )
  {
    if( pauseFrames > 0 )
    {
      pauseFrames--;      
    }
    else
    {
      if( nextPeriodFrames > 0 )
      {
        int xOff = rand() % offset;
        int yOff = rand() % offset;
        DrawGUIRect( -offset + xOff, -offset + yOff , GetResolutionWidth() + xOff, GetResolutionHeight() + yOff, tex, Vector3( 255, 0, 0 ), 120 );

        PlaySoundSource( sound );

        nextPeriodFrames--;
      }
      else
      {
        nextPeriodFrames = 5 + rand() % 10;
        pauseFrames = 10 + rand() % 20;
        PauseSoundSource( sound );
        periods--;        
      }
    }
  }
}
