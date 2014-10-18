#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"

LevelResearchFacility::LevelResearchFacility() {
    scene = LoadScene( "data/maps/release/researchFacility/rf.scene" );

    SetPosition( player->body, GetPosition( FindByName( "PlayerPosition" )));

    SetReverb( 13 );

    steamActivateZone = FindByName( "SteamActivateZone" );

    steamHissSound = CreateSound3D( "data/sounds/steamhiss.ogg" );

    AddLift( lift1 = new Lift( FindByName( "Lift1" ), FindByName( "Lift1Screen"), FindByName( "Lift1Source" ), FindByName("Lift1Dest" ), CreateSound3D( "data/sounds/motor_idle.ogg")));

    fan1 = new Fan( FindByName( "Fan" ), 15, Vector3( 0, 1, 0 ), CreateSound3D( "data/sounds/fan.ogg" ));
    fan2 = new Fan( FindByName( "Fan2" ), 15, Vector3( 0, 1, 0 ), CreateSound3D( "data/sounds/fan.ogg" ));


    scaryBarellThrowZone = FindByName( "ScaryBarellThrowZone" );

    leverSound = CreateSound3D( "data/sounds/lever.ogg");

    AddValve( steamValve = new Valve( FindByName( "SteamValve" ), Vector3( 0, 1, 0 )));
    extemeSteam = new SteamStream( FindByName( "ExtremeSteam"), Vector3( -0.0015, -0.1, -0.0015 ), Vector3( 0.0015, -0.45, 0.0015 ), CreateSound3D( "data/sounds/steamhiss_loop.ogg" ) );

    SetAmbientColor( Vector3( 0, 0, 0 ));

    doorOpenLever = FindByName( "DoorOpenLever" );
    lockedDoor = FindByName( "LockedDoor" );
    spawnRipperZone = FindByName( "SpawnRipperZone" );

    ripperNewPosition = FindByName( "RipperNewPosition");
    repositionRipperZone = FindByName( "RepositionRipperZone");

    extremeSteamBlock = FindByName( "ExtremeSteamBlock" );
    extremeSteamHurtZone = FindByName( "ExtremeSteamHurtZone" );

    CreatePowerUpSequence();

    AddSound( music = CreateMusic( "data/music/rf.ogg" ));
    SetVolume( music, 0.75f );

    AddDoor( new Door( FindInObjectByName( scene, "Door1" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door2" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door3" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door4" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door5" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door6" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door7" ), 90.0f ));
    AddDoor( new Door( FindInObjectByName( scene, "Door8" ), 90.0f ));
}

LevelResearchFacility::~LevelResearchFacility() {
    delete fan1;
    delete fan2;
    delete extemeSteam;
}

void LevelResearchFacility::CreateSteam() {
    steamPS = CreateParticleSystem( 35, GetTexture( "data/textures/particles/p1.png"), PS_STREAM );
    SetParticleSystemSpeedDeviation( steamPS, Vector3( -0.0015, 0.08, -0.0015 ), Vector3( 0.0015, 0.2, 0.0015 ) );
    SetParticleSystemRadius( steamPS, 0.4f );
    SetParticleSystemColors( steamPS, Vector3( 255, 255, 255 ),  Vector3( 255, 255, 255 ) );
    SetParticleSystemPointSize( steamPS, 0.15 );
    SetParticleSystemThickness( steamPS, 1.5 );
    SetPosition( steamPS, GetPosition( FindByName( "RFSteamPos" )));
    AttachSound( steamHissSound, steamPS );
}

void LevelResearchFacility::Show() {
    Level::Show();

    PlaySoundSource( music );
}

void LevelResearchFacility::Hide() {
    Level::Hide();

    PauseSoundSource( music );
}

void LevelResearchFacility::DoScenario() {
    if( Level::curLevelID != LevelName::L3ResearchFacility ) {
        return;
    }

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
        if( IsNodeInside( player->body, steamActivateZone )) {
            CreateSteam();

            PlaySoundSource( steamHissSound, true );

            stateEnterSteamActivateZone = 1;
        }
    }

    if( !stateEnterScaryBarellThrowZone ) {
        if( IsNodeInside( player->body, scaryBarellThrowZone )) {
            SetPosition( FindByName( "ScaryBarell" ), GetPosition( FindByName( "ScaryBarellPos" )));

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

    if( SoundPlaying( steamHissSound ) && steamPS.IsValid() ) {
        static float steamParticleSize = 0.15f;

        SetParticleSystemPointSize( steamPS, steamParticleSize );

        if( steamParticleSize > 0 ) {
            steamParticleSize -= 0.0005f;
        } else {
            FreeSceneNode( steamPS );

            steamPS.Invalidate();
        }
    }

    if( !stateDoorUnlocked ) {
        if( player->nearestPicked == doorOpenLever ) {
            DrawGUIText( "[E] - открыть дверь", GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

            if( mi::KeyDown( mi::E )) {
                SetRotation( doorOpenLever, Quaternion( 0, -20, 0 ));
                SetPosition( lockedDoor, Vector3( 1000, 1000, 1000 )); // far far away :D

                stateDoorUnlocked = true;
            }
        }
    }

    if( stateDoorUnlocked && !stateEnterSpawnRipperZone ) {
        if( IsNodeInside( player->body, spawnRipperZone )) {
            stateEnterSpawnRipperZone = true;

            ripper = new Enemy( "data/models/ripper/ripper.scene" );
            SetPosition( ripper->body, GetPosition( FindByName( "RipperPosition" )));
        }
    }

    if( stateEnterSpawnRipperZone ) {
        ripper->Update();
    }

    if( !stateEnterRepositionRipperZone ) {
        if( IsNodeInside( player->body, repositionRipperZone )) {
            stateEnterRepositionRipperZone = true;
            ripper->detectPlayer = true;
            SetPosition( ripper->body, GetPosition( ripperNewPosition ));
        }
    }

    if( steamValve->done ) {
        SetPosition( extremeSteamBlock, Vector3( 1000, 1000, 1000 ));
    } else {
        if( IsNodeInside( player->body, extremeSteamHurtZone )) {
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
                DrawGUIText( "[E] - вставить предохранитель", GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );
            }
        }

        if( mi::KeyHit( mi::E )) {
            for( int i = 0; i < 3; i++ ) {
                ItemPlace * fuse = fusePlace[i];

                if( fuse->IsPickedByPlayer() ) {
                    bool placed = fuse->PlaceItem( player->inventory.forUse );

                    if( placed ) {
                        ShowNode( fuseModel[i] );
                        fuse->SetPlaceType( -1 );
                    }
                }
            }
        }
    }

    if( fuseInsertedCount >= 3 ) {
        if( player->nearestPicked == powerLever ) {
            DrawGUIText( "[E] - включить питание", GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

            if( mi::KeyHit( mi::E ) && !powerOn ) {
                SetLightColor( FindByName("PowerLamp"), Vector3( 0, 255, 0 ) );

                PlaySoundSource( leverSound, 1 );

                powerSparks = new Sparks( FindByName( "PowerLeverSnd" ), CreateSound3D( "data/sounds/sparks.ogg" ));

                ShowNode( powerLeverOnModel );
                HideNode( powerLeverOffModel );

                //for( int i = 0; i < 6; i++ )
                //  SetLightRange( upLight[i], upLightOnRange[i] );

                powerOn = true;
            }
        }
    }
}

void LevelResearchFacility::CreatePowerUpSequence() {
    AddItem( fuse[0] = new Item( FindInObjectByName( scene, "Fuse1" ), Item::Fuse ));
    AddItem( fuse[1] = new Item( FindInObjectByName( scene, "Fuse2" ), Item::Fuse ));
    AddItem( fuse[2] = new Item( FindInObjectByName( scene, "Fuse3" ), Item::Fuse ));

    AddItemPlace( fusePlace[0] = new ItemPlace( FindInObjectByName( scene, "FusePlace1" ), Item::Fuse ));
    AddItemPlace( fusePlace[1] = new ItemPlace( FindInObjectByName( scene, "FusePlace2" ), Item::Fuse ));
    AddItemPlace( fusePlace[2] = new ItemPlace( FindInObjectByName( scene, "FusePlace3" ), Item::Fuse ));

    fuseModel[0] = FindInObjectByName( scene, "FuseModel1" );
    fuseModel[1] = FindInObjectByName( scene, "FuseModel2" );
    fuseModel[2] = FindInObjectByName( scene, "FuseModel3" );

    powerLeverOnModel = FindInObjectByName( scene, "PowerSwitchOnModel" );
    powerLeverOffModel = FindInObjectByName( scene, "PowerSwitchOffModel" );
    powerLever = FindInObjectByName( scene, "PowerLever" );

    upLight[0] = FindInObjectByName( scene, "UpLight1" );
    upLight[1] = FindInObjectByName( scene, "UpLight2" );
    upLight[2] = FindInObjectByName( scene, "UpLight3" );
    upLight[3] = FindInObjectByName( scene, "UpLight4" );
    upLight[4] = FindInObjectByName( scene, "UpLight5" );
    upLight[5] = FindInObjectByName( scene, "UpLight6" );

    for( int i = 0; i < 6; i++ ) {
        upLightOnRange[i] = GetLightRange( upLight[i] );
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
