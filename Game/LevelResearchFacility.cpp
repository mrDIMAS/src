#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility() {
	typeNum = 4;

    scene = ruLoadScene( "data/maps/release/researchFacility/rf.scene" );

    //SetLocalPosition( player->body, GetPosition( FindInObjectByName( scene, "PlayerPosition" )));
	player->Place( ruGetNodePosition( ruFindInObjectByName( scene, "PlayerPosition" ) ));

    ruSetAudioReverb( 13 );

    steamActivateZone = ruFindInObjectByName( scene, "SteamActivateZone" );

    steamHissSound = ruLoadSound3D( "data/sounds/steamhiss.ogg" );

    AddLift( lift1 = new Lift( ruFindInObjectByName( scene, "Lift1" ), ruFindInObjectByName( scene, "Lift1Screen"), 
		ruFindInObjectByName( scene, "Lift1Source" ), ruFindInObjectByName( scene, "Lift1Dest" ), ruLoadSound3D( "data/sounds/motor_idle.ogg")));

    fan1 = new Fan( ruFindInObjectByName( scene, "Fan" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));
    fan2 = new Fan( ruFindInObjectByName( scene, "Fan2" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));


    scaryBarellThrowZone = ruFindInObjectByName( scene, "ScaryBarellThrowZone" );

    leverSound = ruLoadSound3D( "data/sounds/lever.ogg");

    AddValve( steamValve = new Valve( ruFindInObjectByName( scene, "SteamValve" ), ruVector3( 0, 1, 0 )));
	ruSoundHandle steamHis = ruLoadSound3D( "data/sounds/steamhiss_loop.ogg" ) ;
	AddSound( steamHis );
    extemeSteam = new SteamStream( ruFindInObjectByName( scene, "ExtremeSteam"), ruVector3( -0.0015, -0.1, -0.0015 ), ruVector3( 0.0015, -0.45, 0.0015 ), steamHis );

    ruSetAmbientColor( ruVector3( 0, 0, 0 ));

    doorOpenLever = ruFindInObjectByName( scene, "DoorOpenLever" );
    lockedDoor = ruFindInObjectByName( scene, "LockedDoor" );
    spawnRipperZone = ruFindInObjectByName( scene, "SpawnRipperZone" );

    ripperNewPosition = ruFindInObjectByName( scene, "RipperNewPosition");
    repositionRipperZone = ruFindInObjectByName( scene, "RepositionRipperZone");

    extremeSteamBlock = ruFindInObjectByName( scene, "ExtremeSteamBlock" );
    extremeSteamHurtZone = ruFindInObjectByName( scene, "ExtremeSteamHurtZone" );

    CreatePowerUpSequence();

    AddSound( music = ruLoadMusic( "data/music/rf.ogg" ));
    ruSetSoundVolume( music, 0.75f );

    AddDoor( new Door( ruFindInObjectByName( scene, "Door1" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door2" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door3" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door4" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door5" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door6" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door7" ), 90.0f ));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door8" ), 90.0f ));
}

LevelResearchFacility::~LevelResearchFacility() {
    delete fan1;
    delete fan2;
    delete extemeSteam;
}

void LevelResearchFacility::CreateSteam() {
	ruParticleSystemProperties psProps;
	psProps.texture = ruGetTexture( "data/textures/particles/p1.png");
	psProps.type = PS_STREAM;
	psProps.speedDeviationMin = ruVector3( -0.0015, 0.08, -0.0015 );
	psProps.speedDeviationMax = ruVector3( 0.0015, 0.2, 0.0015 );
	psProps.boundingRadius = 0.4f;
	psProps.colorBegin = ruVector3( 255, 255, 255 );
	psProps.colorEnd = ruVector3( 255, 255, 255 );
	psProps.pointSize = 0.15f;
	psProps.particleThickness = 1.5f;
	psProps.useLighting = false;
    steamPS = ruCreateParticleSystem( 35, psProps );
    ruSetNodePosition( steamPS, ruGetNodePosition( ruFindInObjectByName( scene, "RFSteamPos" )));
    ruAttachSound( steamHissSound, steamPS );
}

void LevelResearchFacility::Show() {
    Level::Show();

    ruPlaySound( music );
}

void LevelResearchFacility::Hide() {
    Level::Hide();

    ruPauseSound( music );
}

void LevelResearchFacility::DoScenario() {
    if( Level::curLevelID != LevelName::L3ResearchFacility ) {
        return;
    }

	ruSetAmbientColor( ruVector3( 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f ));

    static int stateEnterSteamActivateZone = 0;
    static int stateEnterScaryBarellThrowZone = 0;
    static int stateEnterScreamerZone = 0;
    static int stateEnterCoffinOpenZone = 0;
    static int stateEnterSpawnRipperZone = 0;
    static bool stateDoorUnlocked = false;
    static bool stateEnterRepositionRipperZone = false;

    if( powerOn ) {
        lift1->Update();
        fan1->DoTurn();
        fan2->DoTurn();
    }

    if( !stateEnterSteamActivateZone ) {
        if( player->IsInsideZone( steamActivateZone )) {
            CreateSteam();

            ruPlaySound( steamHissSound, true );

            stateEnterSteamActivateZone = 1;
        }
    }

    if( !stateEnterScaryBarellThrowZone ) {
        if( player->IsInsideZone( scaryBarellThrowZone )) {
            ruSetNodePosition( ruFindInObjectByName( scene, "ScaryBarell" ), ruGetNodePosition( ruFindInObjectByName( scene, "ScaryBarellPos" )));

            stateEnterScaryBarellThrowZone = 1;
        }
    }


    steamValve->Update();
    extemeSteam->Update();
    extemeSteam->power = 1 - steamValve->value;

    UpdatePowerupSequence();

    if( powerOn && powerSparks ) {
        powerSparks->Update();

        if( !powerSparks->alive ) {
            delete powerSparks;

            powerSparks = 0;
        }
    }

    if( ruIsSoundPlaying( steamHissSound ) && steamPS.IsValid() ) {
        static float steamParticleSize = 0.15f;

        ruGetParticleSystemProperties( steamPS )->pointSize = steamParticleSize;

        if( steamParticleSize > 0 ) {
            steamParticleSize -= 0.0005f;
        } else {
            ruFreeSceneNode( steamPS );

            steamPS.Invalidate();
        }
    }

    if( !stateDoorUnlocked ) {
        if( player->nearestPicked == doorOpenLever ) {
            ruDrawGUIText( Format( "[%s] - открыть дверь", GetKeyName( player->keyUse )).c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );

            if( ruIsKeyDown( player->keyUse )) {
                ruSetNodeRotation( doorOpenLever, ruQuaternion( 0, -20, 0 ));
                ruSetNodePosition( lockedDoor, ruVector3( 1000, 1000, 1000 )); // far far away :D

                stateDoorUnlocked = true;
            }
        }
    }

    if( stateDoorUnlocked && !stateEnterSpawnRipperZone ) {
        if( player->IsInsideZone( spawnRipperZone )) {
            stateEnterSpawnRipperZone = true;

            //ripper = new Enemy( "data/models/ripper/ripper.scene" );
            //SetPosition( ripper->body, GetPosition( FindByName( "RipperPosition" )));
        }
    }

	if( ripper ) {
		if( stateEnterSpawnRipperZone ) {
			ripper->Think();
		}

		if( !stateEnterRepositionRipperZone ) {
			if( player->IsInsideZone( repositionRipperZone )) {
				stateEnterRepositionRipperZone = true;
				ruSetNodePosition( ripper->body, ruGetNodePosition( ripperNewPosition ));
			}
		}
	}

    if( steamValve->done ) {
        ruSetNodePosition( extremeSteamBlock, ruVector3( 1000, 1000, 1000 ));
    } else {
        if( player->IsInsideZone( extremeSteamHurtZone )) {
            player->Damage( 0.6 );
        }
    }
}

void LevelResearchFacility::UpdatePowerupSequence() {
    if( fuseInsertedCount < 3 ) {
        fuseInsertedCount = 0;

        for( int i = 0; i < 3; i++ ) {
            ItemPlace * fuse = fusePlace[i];

            if( fuse->GetPlaceType() == -1 ) {
                fuseInsertedCount++;
            }
        }
    }

    if( player->inventory.forUse ) {
        for( int i = 0; i < 3; i++ ) {
            ItemPlace * fuse = fusePlace[i];

            if( fuse->IsPickedByPlayer() ) {
                ruDrawGUIText( Format( "[%s] - вставить предохранитель", GetKeyName( player->keyUse )).c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
            }
        }

        if( ruIsKeyHit( player->keyUse )) {
            for( int i = 0; i < 3; i++ ) {
                ItemPlace * fuse = fusePlace[i];

                if( fuse->IsPickedByPlayer() ) {
                    bool placed = fuse->PlaceItem( player->inventory.forUse );

                    if( placed ) {
                        ruShowNode( fuseModel[i] );
                        fuse->SetPlaceType( -1 );
                    }
                }
            }
        }
    }

    if( fuseInsertedCount >= 3 ) {
        if( player->nearestPicked == powerLever ) {
            ruDrawGUIText( Format( player->localization.GetString("powerUp"), GetKeyName( player->keyUse )).c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );

            if( ruIsKeyHit( player->keyUse ) && !powerOn ) {
                ruSetLightColor( ruFindInObjectByName( scene, "PowerLamp"), ruVector3( 0, 255, 0 ) );

                ruPlaySound( leverSound, 1 );

                powerSparks = new Sparks( ruFindInObjectByName( scene, "PowerLeverSnd" ), ruLoadSound3D( "data/sounds/sparks.ogg" ));

                ruShowNode( powerLeverOnModel );
                ruHideNode( powerLeverOffModel );

                //for( int i = 0; i < 6; i++ )
                //  SetLightRange( upLight[i], upLightOnRange[i] );

                powerOn = true;
            }
        }
    }
}

void LevelResearchFacility::CreatePowerUpSequence() {
    AddItem( fuse[0] = new Item( ruFindInObjectByName( scene, "Fuse1" ), Item::Fuse ));
    AddItem( fuse[1] = new Item( ruFindInObjectByName( scene, "Fuse2" ), Item::Fuse ));
    AddItem( fuse[2] = new Item( ruFindInObjectByName( scene, "Fuse3" ), Item::Fuse ));

    AddItemPlace( fusePlace[0] = new ItemPlace( ruFindInObjectByName( scene, "FusePlace1" ), Item::Fuse ));
    AddItemPlace( fusePlace[1] = new ItemPlace( ruFindInObjectByName( scene, "FusePlace2" ), Item::Fuse ));
    AddItemPlace( fusePlace[2] = new ItemPlace( ruFindInObjectByName( scene, "FusePlace3" ), Item::Fuse ));

    fuseModel[0] = ruFindInObjectByName( scene, "FuseModel1" );
    fuseModel[1] = ruFindInObjectByName( scene, "FuseModel2" );
    fuseModel[2] = ruFindInObjectByName( scene, "FuseModel3" );

    powerLeverOnModel = ruFindInObjectByName( scene, "PowerSwitchOnModel" );
    powerLeverOffModel = ruFindInObjectByName( scene, "PowerSwitchOffModel" );
    powerLever = ruFindInObjectByName( scene, "PowerLever" );

    upLight[0] = ruFindInObjectByName( scene, "UpLight1" );
    upLight[1] = ruFindInObjectByName( scene, "UpLight2" );
    upLight[2] = ruFindInObjectByName( scene, "UpLight3" );
    upLight[3] = ruFindInObjectByName( scene, "UpLight4" );
    upLight[4] = ruFindInObjectByName( scene, "UpLight5" );
    upLight[5] = ruFindInObjectByName( scene, "UpLight6" );

    for( int i = 0; i < 6; i++ ) {
        upLightOnRange[i] = ruGetLightRange( upLight[i] );
        //SetLightRange( upLight[i], 0.0f );
    }

    fuseInsertedCount = 0;

    powerOn = false;
}

void LevelResearchFacility::OnDeserialize( TextFileStream & in )
{

}

void LevelResearchFacility::OnSerialize( TextFileStream & out )
{

}
