#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( ) {
	typeNum = 2;

    // Load localization
    LoadLocalization( "arrival.loc" );

    // Load main scene
    scene = ruLoadScene( "data/maps/release/arrival/arrival.scene" );

    //////////////////////////////////////////////////////////////////////////
    // Find and create all sheets
    AddSheet( new Sheet( ruFindByName( "Note1" ), localization.GetString( "note1Desc" ), localization.GetString( "note1" )));

    //////////////////////////////////////////////////////////////////////////
    // Find zones
    strangeSoundZone = ruFindByName( "StrangeSoundZone" );

    nextLevelLoadZone = ruFindByName( "NextLevelLoadZone" );

    rocksFallZone = ruFindByName( "RocksFallZone");


    //////////////////////////////////////////////////////////////////////////
    // Find entities
    rocks = ruFindByName( "Rocks" );
    rocksPos = ruFindByName( "RocksPos" );
    generator = ruFindByName( "Generator" );

    lamp1 = ruFindByName( "Projector1" );
    lamp2 = ruFindByName( "Projector2" );

    //////////////////////////////////////////////////////////////////////////
    // Player noticements
    player->SetObjective( localization.GetString( "objective1" ));
    player->SetPlaceDescription( localization.GetString( "placeDesc" ));

    ruSetNodePosition( player->body, ruGetNodePosition( ruFindByName("PlayerPosition")) + ruVector3( 0, 1, 0 ));

    //////////////////////////////////////////////////////////////////////////
    // Load sounds
    AddSound( generatorSound = ruLoadSound3D( "data/sounds/generator.ogg" ));
    ruAttachSound( generatorSound, ruFindByName( "Generator" ) );
    ruSetRolloffFactor( generatorSound, 5 );
    ruSetSoundReferenceDistance( generatorSound, 5 );

    AddSound( windSound = ruLoadSound2D( "data/sounds/wind.ogg"));
    ruSetSoundVolume( windSound, 0.5f );
    ruPlaySound( windSound );

    AddSound( explosionSound = ruLoadSound2D( "data/sounds/blast.ogg" ));

    AddSound( strangeSound = ruLoadSound3D( "data/sounds/eerie3.ogg" ));
    ruSetRolloffFactor( strangeSound, 5 );
    ruSetSoundReferenceDistance( strangeSound, 15 );
    ruAttachSound( strangeSound, generator );

    ruSetAudioReverb( 15 );

    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient1.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient2.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient3.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient4.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient5.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/forest/forestambient6.ogg" ));

    player->SetFootsteps( FootstepsType::Dirt );

    stages[ "DoneStrangeSoundPlayed" ] = false;
    stages[ "DoneRocksFall" ] = false;

	ruSetCameraSkybox( player->camera->cameraNode, "data/textures/skyboxes/night4/nnksky01");
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

	ruSetAmbientColor( ruVector3( 0.06, 0.06, 0.06 ));

    PlayAmbientSounds();

    ruPlaySound( generatorSound );

    if( !stages[ "DoneStrangeSoundPlayed" ] ) {
        if( ruIsNodeInsideNode( player->body, strangeSoundZone )) {
            ruPlaySound( strangeSound );

            stages[ "DoneStrangeSoundPlayed" ] = true;
        }
    }

    if( !stages[ "DoneRocksFall" ] ) {
        if( ruIsNodeInsideNode( player->body, rocksFallZone )) {
            ruSetNodePosition( rocks, ruGetNodePosition( rocksPos ) );

            ruSetLightRange( lamp1, 0 );
            ruSetLightRange( lamp2, 0 );

            ruPlaySound( explosionSound );

            player->SetObjective( localization.GetString( "objective4" ) );

            AddSound( music = ruLoadMusic( "data/sounds/fear.ogg" ));
            ruPlaySound( music );

            stages[ "DoneRocksFall" ] = true;
        }
    }

    if( ruIsNodeInsideNode( player->body, nextLevelLoadZone )) {
        Level::Change( LevelName::L2Mine );
    }
}
