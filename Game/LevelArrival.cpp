#include "LevelArrival.h"
#include "Player.h"
#include "LevelMine.h"


LevelArrival::LevelArrival( ) {
    mTypeNum = 2;

    // Load localization
    LoadLocalization( "arrival.loc" );

    // Load main scene
    LoadSceneFromFile( "data/maps/release/arrival/arrival.scene" );

    //////////////////////////////////////////////////////////////////////////
    // Find and create all sheets
    AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" )));

    //////////////////////////////////////////////////////////////////////////
    // Find zones
    strangeSoundZone = GetUniqueObject( "StrangeSoundZone" );
    nextLevelLoadZone = GetUniqueObject( "NextLevelLoadZone" );
    rocksFallZone = GetUniqueObject( "RocksFallZone");


    //////////////////////////////////////////////////////////////////////////
    // Find entities
    rocks = GetUniqueObject( "Rocks" );
    rocksPos = GetUniqueObject( "RocksPos" );
    generator = GetUniqueObject( "Generator" );

    lamp1 = GetUniqueObject( "Projector1" );
    lamp2 = GetUniqueObject( "Projector2" );

    //////////////////////////////////////////////////////////////////////////
    // Player noticements
    pPlayer->SetObjective( mLocalization.GetString( "objective1" ));

    pPlayer->SetPosition( ruGetNodePosition( GetUniqueObject("PlayerPosition")) + ruVector3( 0, 1, 0 ));

    //////////////////////////////////////////////////////////////////////////
    // Load sounds
    AddSound( generatorSound = ruLoadSound3D( "data/sounds/generator.ogg" ));
    ruAttachSound( generatorSound, GetUniqueObject( "Generator" ) );
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

    pPlayer->SetFootsteps( FootstepsType::Dirt );

    mStages[ "DoneStrangeSoundPlayed" ] = false;
    mStages[ "DoneRocksFall" ] = false;

    ruSetCameraSkybox( pPlayer->mpCamera->mNode, "data/textures/skyboxes/night4/nnksky01" );

    DoneInitialization();
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
    if( Level::msCurLevelID != LevelName::L1Arrival ) {
        return;
    }

    ruSetAmbientColor( ruVector3( 0.11, 0.11, 0.11 ));

    PlayAmbientSounds();

    ruPlaySound( generatorSound );

    if( !mStages[ "DoneStrangeSoundPlayed" ] ) {
        if( pPlayer->IsInsideZone( strangeSoundZone )) {
            ruPlaySound( strangeSound );

            mStages[ "DoneStrangeSoundPlayed" ] = true;
        }
    }

    if( !mStages[ "DoneRocksFall" ] ) {
        if( pPlayer->IsInsideZone( rocksFallZone )) {
            ruSetNodePosition( rocks, ruGetNodePosition( rocksPos ) );

            ruSetLightRange( lamp1, 0 );
            ruSetLightRange( lamp2, 0 );

            ruPlaySound( explosionSound );

            pPlayer->SetObjective( mLocalization.GetString( "objective4" ) );

            AddSound( mMusic = ruLoadMusic( "data/sounds/fear.ogg" ));
            ruPlaySound( mMusic );

            mStages[ "DoneRocksFall" ] = true;
        }
    }

    if( pPlayer->IsInsideZone( nextLevelLoadZone )) {
        Level::Change( LevelName::L2Mine );
    }
}
