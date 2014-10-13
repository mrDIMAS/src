#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( )
{
  // Load localization
  ParseFile( "data/lang/rus/arrival.loc", loc );

  // Load main scene
  scene = LoadScene( "data/maps/release/arrival/arrival.scene" );  

  //////////////////////////////////////////////////////////////////////////
  // Find and create all sheets
  AddSheet( new Sheet( FindByName( "Note1" ), loc[ "note1Desc" ], loc[ "note1" ] ) );  

  //////////////////////////////////////////////////////////////////////////
  // Find zones
  strangeSoundZone = FindByName( "StrangeSoundZone" );

  nextLevelLoadZone = FindByName( "NextLevelLoadZone" );

  rocksFallZone = FindByName( "RocksFallZone");


  //////////////////////////////////////////////////////////////////////////
  // Find entities
  rocks = FindByName( "Rocks" );
  rocksPos = FindByName( "RocksPos" );  
  generator = FindByName( "Generator" );

  lamp1 = FindByName( "Projector1" );
  lamp2 = FindByName( "Projector2" );  

  //////////////////////////////////////////////////////////////////////////
  // Player noticements
  player->SetObjective( loc[ "objective1" ] );  
  player->SetPlaceDescription( loc[ "placeDesc" ] );

  SetPosition( player->body, GetPosition( FindByName("PlayerPosition")) + Vector3( 0, 1, 0 ));

  //////////////////////////////////////////////////////////////////////////
  // Load sounds

  creepyMus = CreateSound2D( "data/sounds/stress.ogg");

  generatorSound = CreateSound3D( "data/sounds/generator.ogg" );
  AttachSound( generatorSound, FindByName( "Generator" ) );
 
  windSound = CreateSound2D( "data/sounds/wind.ogg");
  SetVolume( windSound, 0.5f );
  PlaySoundSource( windSound );

  explosionSound = CreateSound2D( "data/sounds/blast.ogg" );

  strangeSound = CreateSound3D( "data/sounds/eerie3.ogg" );
  AttachSound( strangeSound, generator );
  SetVolume( strangeSound, 2 );


  SetReverb( 15 );

  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient1.ogg" ));
  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient2.ogg" ));
  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient3.ogg" ));
  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient4.ogg" ));
  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient5.ogg" ));
  ambSoundSet.AddSound( CreateSound3D( "data/sounds/ambient/forest/forestambient6.ogg" ));

  player->SetFootsteps( FootstepsType::Dirt );

  stages[ "DoneStrangeSoundPlayed" ] = false;
  stages[ "DoneRocksFall" ] = false;
}

LevelArrival::~LevelArrival()
{
  FreeSoundSource( windSound );
  FreeSoundSource( generatorSound );
  FreeSoundSource( creepyMus );
  FreeSoundSource( explosionSound );
  FreeSoundSource( music );
}

void LevelArrival::Show()
{
  Level::Show();

  PlaySoundSource( windSound );
}

void LevelArrival::Hide()
{
  Level::Hide();

  PauseSoundSource( windSound );
  PauseSoundSource( generatorSound );
}

void LevelArrival::DoScenario()
{
  if( Level::currentLevelName != LevelName::L1Arrival )
    return;

  ambSoundSet.DoRandomPlaying();

    PlaySoundSource( generatorSound );

  if( !stages[ "DoneStrangeSoundPlayed" ] )
  {
    if( IsNodeInside( player->body, strangeSoundZone ))
    {
      PlaySoundSource( strangeSound );

      stages[ "DoneStrangeSoundPlayed" ] = true; 
    }
  }

  if( !stages[ "DoneRocksFall" ] )
  {
    if( IsNodeInside( player->body, rocksFallZone ))
    {
      SetPosition( rocks, GetPosition( rocksPos ) );

      SetLightRange( lamp1, 0 );
      SetLightRange( lamp2, 0 );
 
      PlaySoundSource( explosionSound );    

      player->SetObjective( loc[ "objective4" ] );

      music = CreateMusic( "data/sounds/fear.ogg" );
      PlaySoundSource( music );

      stages[ "DoneRocksFall" ] = true;
    }
  }

  if( IsNodeInside( player->body, nextLevelLoadZone ))
  {
    Level::Change( LevelName::L2Mine );
  }
}
