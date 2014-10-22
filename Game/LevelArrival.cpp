#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( ) {
    // Load localization
    LoadLocalization( "arrival.loc" );

    // Load main scene
    scene = LoadScene( "data/maps/release/arrival/arrival.scene" );

    //////////////////////////////////////////////////////////////////////////
    // Find and create all sheets
    AddSheet( new Sheet( FindByName( "Note1" ), localization.GetString( "note1Desc" ), localization.GetString( "note1" )));

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
    player->SetObjective( localization.GetString( "objective1" ));
    player->SetPlaceDescription( localization.GetString( "placeDesc" ));

    SetPosition( player->body, GetPosition( FindByName("PlayerPosition")) + Vector3( 0, 1, 0 ));

    //////////////////////////////////////////////////////////////////////////
    // Load sounds
    AddSound( generatorSound = CreateSound3D( "data/sounds/generator.ogg" ));
    AttachSound( generatorSound, FindByName( "Generator" ) );
    SetRolloffFactor( generatorSound, 5 );
    SetSoundReferenceDistance( generatorSound, 5 );

    AddSound( windSound = CreateSound2D( "data/sounds/wind.ogg"));
    SetVolume( windSound, 0.5f );
    PlaySoundSource( windSound );

    AddSound( explosionSound = CreateSound2D( "data/sounds/blast.ogg" ));

    AddSound( strangeSound = CreateSound3D( "data/sounds/eerie3.ogg" ));
    SetRolloffFactor( strangeSound, 5 );
    SetSoundReferenceDistance( strangeSound, 15 );
    AttachSound( strangeSound, generator );

    SetReverb( 15 );

    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient1.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient2.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient3.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient4.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient5.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/forest/forestambient6.ogg" ));

    player->SetFootsteps( FootstepsType::Dirt );

    stages[ "DoneStrangeSoundPlayed" ] = false;
    stages[ "DoneRocksFall" ] = false;

}

LevelArrival::~LevelArrival() {

}

void LevelArrival::Show() {
    Level::Show();
}

void LevelArrival::Hide() {
    Level::Hide();
}

void LevelArrival::DoScenario() {
    if( Level::curLevelID != LevelName::L1Arrival ) {
        return;
    }

    PlayAmbientSounds();

    PlaySoundSource( generatorSound );

    if( !stages[ "DoneStrangeSoundPlayed" ] ) {
        if( IsNodeInside( player->body, strangeSoundZone )) {
            PlaySoundSource( strangeSound );

            stages[ "DoneStrangeSoundPlayed" ] = true;
        }
    }

    if( !stages[ "DoneRocksFall" ] ) {
        if( IsNodeInside( player->body, rocksFallZone )) {
            SetPosition( rocks, GetPosition( rocksPos ) );

            SetLightRange( lamp1, 0 );
            SetLightRange( lamp2, 0 );

            PlaySoundSource( explosionSound );

            player->SetObjective( localization.GetString( "objective4" ) );

            AddSound( music = CreateMusic( "data/sounds/fear.ogg" ));
            PlaySoundSource( music );

            stages[ "DoneRocksFall" ] = true;
        }
    }

    if( IsNodeInside( player->body, nextLevelLoadZone )) {
        Level::Change( LevelName::L2Mine );
    }
}
