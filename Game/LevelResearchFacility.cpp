#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility() {
    mTypeNum = 4;

    LoadSceneFromFile( "data/maps/release/researchFacility/rf.scene" );

    pPlayer->SetPosition( ruGetNodePosition( GetUniqueObject( "PlayerPosition" ) ));

    ruSetAudioReverb( 13 );

    mSteamActivateZone = GetUniqueObject( "SteamActivateZone" );

    AddSound( mSteamHissSound = ruLoadSound3D( "data/sounds/steamhiss.ogg" ));

    mLift1 = new Lift( GetUniqueObject( "Lift1" ) );
    mLift1->SetControlPanel( GetUniqueObject( "Lift1Screen" ));
    mLift1->SetDestinationPoint( GetUniqueObject( "Lift1Dest" ));
    mLift1->SetSourcePoint( GetUniqueObject( "Lift1Source" ));
    mLift1->SetMotorSound( ruLoadSound3D( "data/sounds/motor_idle.ogg"));
    mLift1->SetBackDoors( GetUniqueObject( "Lift1BackDoorLeft"), GetUniqueObject( "Lift1BackDoorRight" ));
    mLift1->SetFrontDoors( GetUniqueObject( "Lift1FrontDoorLeft"), GetUniqueObject( "Lift1FrontDoorRight" ));
    AddLift( mLift1 );

    mpFan1 = new Fan( GetUniqueObject( "Fan" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));
    mpFan2 = new Fan( GetUniqueObject( "Fan2" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));


    mScaryBarellThrowZone = GetUniqueObject( "ScaryBarellThrowZone" );

    AddSound( mLeverSound = ruLoadSound3D( "data/sounds/lever.ogg"));

    AddValve( mpSteamValve = new Valve( GetUniqueObject( "SteamValve" ), ruVector3( 0, 1, 0 )));
    ruSoundHandle steamHis = ruLoadSound3D( "data/sounds/steamhiss_loop.ogg" ) ;
    AddSound( steamHis );
    mpExtemeSteam = new SteamStream( GetUniqueObject( "ExtremeSteam"), ruVector3( -0.0015, -0.1, -0.0015 ), ruVector3( 0.0015, -0.45, 0.0015 ), steamHis );

    ruSetAmbientColor( ruVector3( 0, 0, 0 ));

    mDoorOpenLever = GetUniqueObject( "DoorOpenLever" );
    mLockedDoor = GetUniqueObject( "LockedDoor" );
    mSpawnRipperZone = GetUniqueObject( "SpawnRipperZone" );

    mRipperNewPosition = GetUniqueObject( "RipperNewPosition");
    mRepositionRipperZone = GetUniqueObject( "RepositionRipperZone");

    mExtremeSteamBlock = GetUniqueObject( "ExtremeSteamBlock" );
    mExtremeSteamHurtZone = GetUniqueObject( "ExtremeSteamHurtZone" );

    CreatePowerUpSequence();

    AddSound( mMusic = ruLoadMusic( "data/music/rf.ogg" ));
    ruSetSoundVolume( mMusic, 0.75f );

    AddDoor( new Door( GetUniqueObject( "Door1" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door2" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door3" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door4" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door5" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door6" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door7" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door8" ), 90.0f ));

    mScaryBarell = GetUniqueObject( "ScaryBarell" );
    mScaryBarellPositionNode = GetUniqueObject( "ScaryBarellPos" );
    mPowerLamp = GetUniqueObject( "PowerLamp");
    mPowerLeverSnd = GetUniqueObject( "PowerLeverSnd");
    mSmallSteamPosition = GetUniqueObject( "RFSteamPos" );

    DoneInitialization();
}

LevelResearchFacility::~LevelResearchFacility() {
    delete mpFan1;
    delete mpFan2;
    delete mpExtemeSteam;
}


void LevelResearchFacility::Show() {
    Level::Show();

    ruPlaySound( mMusic );
}

void LevelResearchFacility::Hide() {
    Level::Hide();

    ruPauseSound( mMusic );
}

void LevelResearchFacility::DoScenario() {
    if( Level::msCurLevelID != LevelName::L3ResearchFacility ) {
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

    if( mPowerOn ) {
        mLift1->Update();
        mpFan1->DoTurn();
        mpFan2->DoTurn();
    }

    if( !stateEnterSteamActivateZone ) {
        if( pPlayer->IsInsideZone( mSteamActivateZone )) {
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
            mSteamPS = ruCreateParticleSystem( 35, psProps );
            ruSetNodePosition( mSteamPS, ruGetNodePosition( mSmallSteamPosition ));
            ruAttachSound( mSteamHissSound, mSteamPS );

            ruPlaySound( mSteamHissSound, true );

            stateEnterSteamActivateZone = 1;
        }
    }

    if( !stateEnterScaryBarellThrowZone ) {
        if( pPlayer->IsInsideZone( mScaryBarellThrowZone )) {
            ruSetNodePosition( mScaryBarell, ruGetNodePosition( mScaryBarellPositionNode ));

            stateEnterScaryBarellThrowZone = 1;
        }
    }


    mpSteamValve->Update();
    mpExtemeSteam->Update();
    mpExtemeSteam->power = 1.0f - mpSteamValve->GetClosedCoeffecient();;

    UpdatePowerupSequence();

    if( mPowerOn && mpPowerSparks ) {
        mpPowerSparks->Update();

        if( !mpPowerSparks->alive ) {
            delete mpPowerSparks;

            mpPowerSparks = 0;
        }
    }

    if( ruIsSoundPlaying( mSteamHissSound ) && mSteamPS.IsValid() ) {
        static float steamParticleSize = 0.15f;

        ruGetParticleSystemProperties( mSteamPS )->pointSize = steamParticleSize;

        if( steamParticleSize > 0 ) {
            steamParticleSize -= 0.0005f;
        } else {
            ruFreeSceneNode( mSteamPS );

            mSteamPS.Invalidate();
        }
    }

    if( !stateDoorUnlocked ) {
        if( pPlayer->mNearestPickedNode == mDoorOpenLever ) {
            pPlayer->SetActionText( Format( pPlayer->mLocalization.GetString( "openGates" ), GetKeyName( pPlayer->mKeyUse )).c_str());

            if( ruIsKeyDown( pPlayer->mKeyUse )) {
                ruSetNodeRotation( mDoorOpenLever, ruQuaternion( 0, -20, 0 ));
                ruSetNodePosition( mLockedDoor, ruVector3( 1000, 1000, 1000 )); // far far away :D

                stateDoorUnlocked = true;
            }
        }
    }

    if( stateDoorUnlocked && !stateEnterSpawnRipperZone ) {
        if( pPlayer->IsInsideZone( mSpawnRipperZone )) {
            stateEnterSpawnRipperZone = true;

            //ripper = new Enemy( "data/models/ripper/ripper.scene" );
            //SetPosition( ripper->body, GetPosition( FindByName( "RipperPosition" )));
        }
    }

    if( mpRipper ) {
        if( stateEnterSpawnRipperZone ) {
            mpRipper->Think();
        }

        if( !stateEnterRepositionRipperZone ) {
            if( pPlayer->IsInsideZone( mRepositionRipperZone )) {
                stateEnterRepositionRipperZone = true;
                mpRipper->SetPosition( ruGetNodePosition( mRipperNewPosition ));
            }
        }
    }

    if( mpSteamValve->IsDone() ) {
        ruSetNodePosition( mExtremeSteamBlock, ruVector3( 1000, 1000, 1000 ));
    } else {
        if( pPlayer->IsInsideZone( mExtremeSteamHurtZone )) {
            pPlayer->Damage( 0.6 );
        }
    }
}

void LevelResearchFacility::UpdatePowerupSequence() {
    if( fuseInsertedCount < 3 ) {
        fuseInsertedCount = 0;

        for( int iFuse = 0; iFuse < 3; iFuse++ ) {
            ItemPlace * pFuse = mFusePlaceList[iFuse];
            if( pFuse->GetPlaceType() == Item::Type::Unknown ) {
                fuseInsertedCount++;
            }
        }
    }

    if( pPlayer->GetInventory()->GetItemSelectedForUse() ) {
        for( int iFuse = 0; iFuse < 3; iFuse++ ) {
            ItemPlace * pFuse = mFusePlaceList[iFuse];
            if( pFuse->IsPickedByPlayer() ) {
                pPlayer->SetActionText( Format( pPlayer->GetLocalization()->GetString( "insertFuse" ), GetKeyName( pPlayer->mKeyUse )).c_str());
            }
        }

        if( ruIsKeyHit( pPlayer->mKeyUse )) {
            for( int iFusePlace = 0; iFusePlace < 3; iFusePlace++ ) {
                ItemPlace * pFuse = mFusePlaceList[iFusePlace];

                if( pFuse->IsPickedByPlayer() ) {
                    bool placed = pFuse->PlaceItem( pPlayer->mInventory.GetItemSelectedForUse() );

                    if( placed ) {
                        ruShowNode( fuseModel[iFusePlace] );
                        pFuse->SetPlaceType( Item::Type::Unknown );
                    }
                }
            }
        }
    }

    if( fuseInsertedCount >= 3 ) {
        if( pPlayer->mNearestPickedNode == powerLever ) {
            pPlayer->SetActionText( Format( pPlayer->mLocalization.GetString("powerUp"), GetKeyName( pPlayer->mKeyUse )).c_str());

            if( ruIsKeyHit( pPlayer->mKeyUse ) && !mPowerOn ) {
                ruSetLightColor( mPowerLamp, ruVector3( 0, 255, 0 ) );

                ruPlaySound( mLeverSound, 1 );

                mpPowerSparks = new Sparks( mPowerLeverSnd, ruLoadSound3D( "data/sounds/sparks.ogg" ));

                ruShowNode( mPowerLeverOnModel );
                ruHideNode( mPowerLeverOffModel );

                mPowerOn = true;
            }
        }
    }
}

void LevelResearchFacility::CreatePowerUpSequence() {
    AddItem( fuse[0] = new Item( GetUniqueObject( "Fuse1" ), Item::Type::Fuse ));
    AddItem( fuse[1] = new Item( GetUniqueObject( "Fuse2" ), Item::Type::Fuse ));
    AddItem( fuse[2] = new Item( GetUniqueObject( "Fuse3" ), Item::Type::Fuse ));

    AddItemPlace( mFusePlaceList[0] = new ItemPlace( GetUniqueObject( "FusePlace1" ), Item::Type::Fuse ));
    AddItemPlace( mFusePlaceList[1] = new ItemPlace( GetUniqueObject( "FusePlace2" ), Item::Type::Fuse ));
    AddItemPlace( mFusePlaceList[2] = new ItemPlace( GetUniqueObject( "FusePlace3" ), Item::Type::Fuse ));

    fuseModel[0] = GetUniqueObject( "FuseModel1" );
    fuseModel[1] = GetUniqueObject( "FuseModel2" );
    fuseModel[2] = GetUniqueObject( "FuseModel3" );

    mPowerLeverOnModel = GetUniqueObject( "PowerSwitchOnModel" );
    mPowerLeverOffModel = GetUniqueObject( "PowerSwitchOffModel" );
    powerLever = GetUniqueObject( "PowerLever" );

    fuseInsertedCount = 0;

    mPowerOn = false;
}

void LevelResearchFacility::OnDeserialize( TextFileStream & in ) {

}

void LevelResearchFacility::OnSerialize( TextFileStream & out ) {

}
