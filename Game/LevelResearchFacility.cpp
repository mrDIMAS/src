#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility() {
    mTypeNum = 4;

    LoadSceneFromFile( "data/maps/release/researchFacility/rf.scene" );
	LoadLocalization( "rf.loc" );

    pPlayer->SetPosition( ruGetNodePosition( GetUniqueObject( "PlayerPosition" ) ));
	
	AddSound( mSteamHissSound = ruLoadSound3D( "data/sounds/steamhiss.ogg" ));
	ruSetRolloffFactor( mSteamHissSound, 5 );
	ruSetSoundReferenceDistance( mSteamHissSound, 4 );
	ruSetRoomRolloffFactor( mSteamHissSound, 2.5f );

    mLift1 = new Lift( GetUniqueObject( "Lift1" ) );
    mLift1->SetControlPanel( GetUniqueObject( "Lift1Screen" ));
    mLift1->SetDestinationPoint( GetUniqueObject( "Lift1Dest" ));
    mLift1->SetSourcePoint( GetUniqueObject( "Lift1Source" ));
    mLift1->SetMotorSound( ruLoadSound3D( "data/sounds/motor_idle.ogg"));
    mLift1->SetBackDoors( GetUniqueObject( "Lift1BackDoorLeft"), GetUniqueObject( "Lift1BackDoorRight" ));
    mLift1->SetFrontDoors( GetUniqueObject( "Lift1FrontDoorLeft"), GetUniqueObject( "Lift1FrontDoorRight" ));
    AddLift( mLift1 );
	
	mLift2 = new Lift( GetUniqueObject( "Lift2" ) );
	mLift2->SetControlPanel( GetUniqueObject( "Lift2Screen" ));
	mLift2->SetDestinationPoint( GetUniqueObject( "Lift2Dest" ));
	mLift2->SetSourcePoint( GetUniqueObject( "Lift2Source" ));
	mLift2->SetMotorSound( ruLoadSound3D( "data/sounds/motor_idle.ogg"));
	mLift2->SetBackDoors( GetUniqueObject( "Lift2BackDoorLeft"), GetUniqueObject( "Lift2BackDoorRight" ));
	mLift2->SetFrontDoors( GetUniqueObject( "Lift2FrontDoorLeft"), GetUniqueObject( "Lift2FrontDoorRight" ));
	AddLift( mLift2 );

    mpFan1 = new Fan( GetUniqueObject( "Fan" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));
    mpFan2 = new Fan( GetUniqueObject( "Fan2" ), 15, ruVector3( 0, 1, 0 ), ruLoadSound3D( "data/sounds/fan.ogg" ));

	AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note2" ), mLocalization.GetString( "note2Desc" ), mLocalization.GetString( "note2" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note3" ), mLocalization.GetString( "note3Desc" ), mLocalization.GetString( "note3" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note4" ), mLocalization.GetString( "note4Desc" ), mLocalization.GetString( "note4" ) ) );

    AddSound( mLeverSound = ruLoadSound3D( "data/sounds/lever.ogg"));

    AddValve( mpSteamValve = new Valve( GetUniqueObject( "SteamValve" ), ruVector3( 0, 1, 0 )));
    ruSoundHandle steamHis = ruLoadSound3D( "data/sounds/steamhiss_loop.ogg" ) ;
	ruSetRolloffFactor( steamHis, 5 );
	ruSetSoundReferenceDistance( steamHis, 4 );
	ruSetRoomRolloffFactor( steamHis, 2.5f );
    AddSound( steamHis );
    mpExtemeSteam = new SteamStream( GetUniqueObject( "ExtremeSteam"), ruVector3( -0.0015, -0.1, -0.0015 ), ruVector3( 0.0015, -0.45, 0.0015 ), steamHis );

    ruSetAmbientColor( ruVector3( 0, 0, 0 ));

	mDoorUnderFloor = GetUniqueObject( "DoorUnderFloor" );
    mDoorOpenLever = GetUniqueObject( "DoorOpenLever" );
    mLockedDoor = GetUniqueObject( "LockedDoor" );

	mZoneScaryBarellThrow = GetUniqueObject( "ScaryBarellThrowZone" );
	mZoneSteamActivate = GetUniqueObject( "SteamActivateZone" );
    mExtremeSteamBlock = GetUniqueObject( "ExtremeSteamBlock" );
    mZoneExtremeSteamHurt = GetUniqueObject( "ExtremeSteamHurtZone" );
	mZoneObjectiveRestorePower = GetUniqueObject( "ObjectiveRestorePower" );
	mZoneExaminePlace = GetUniqueObject( "ObjectiveExaminePlace" );
	mZoneNeedCrowbar = GetUniqueObject( "ObjectiveNeedCrowbar" );
	mZoneObjectiveNeedPassThroughMesh = GetUniqueObject( "ObjectiveNeedPassThroughMesh" );

    CreatePowerUpSequence();

    AddSound( mMusic = ruLoadMusic( "data/music/rf.ogg" ));
    ruSetSoundVolume( mMusic, 0.75f );
	
	AddLadder( new Ladder( GetUniqueObject( "LadderBegin" ), GetUniqueObject( "LadderEnd" ), GetUniqueObject( "LadderEnter" ),
		GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder2Begin" ), GetUniqueObject( "Ladder2End" ), GetUniqueObject( "Ladder2Enter" ),
		GetUniqueObject( "Ladder2BeginLeavePoint"), GetUniqueObject( "Ladder2EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder4Begin" ), GetUniqueObject( "Ladder4End" ), GetUniqueObject( "Ladder4Enter" ),
		GetUniqueObject( "Ladder4BeginLeavePoint"), GetUniqueObject( "Ladder4EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder5Begin" ), GetUniqueObject( "Ladder5End" ), GetUniqueObject( "Ladder5Enter" ),
		GetUniqueObject( "Ladder5BeginLeavePoint"), GetUniqueObject( "Ladder5EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder6Begin" ), GetUniqueObject( "Ladder6End" ), GetUniqueObject( "Ladder6Enter" ),
		GetUniqueObject( "Ladder6BeginLeavePoint"), GetUniqueObject( "Ladder6EndLeavePoint")));
	AddLadder( new Ladder( GetUniqueObject( "Ladder7Begin" ), GetUniqueObject( "Ladder7End" ), GetUniqueObject( "Ladder7Enter" ),
		GetUniqueObject( "Ladder7BeginLeavePoint"), GetUniqueObject( "Ladder7EndLeavePoint")));
    AddDoor( mKeypad3DoorToUnlock = new Door( GetUniqueObject( "Door4" ), 90.0f ));
    AddDoor( mKeypad1DoorToUnlock = new Door( GetUniqueObject( "Door5" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door6" ), 90.0f ));
    AddDoor( mKeypad2DoorToUnlock = new Door( GetUniqueObject( "Door8" ), 90.0f ));
    AddDoor( new Door( GetUniqueObject( "Door009" ), 90.0f ));
    mScaryBarell = GetUniqueObject( "ScaryBarell" );
    mScaryBarellPositionNode = GetUniqueObject( "ScaryBarellPos" );
    mPowerLamp = GetUniqueObject( "PowerLamp");
    mPowerLeverSnd = GetUniqueObject( "PowerLeverSnd");
    mSmallSteamPosition = GetUniqueObject( "RFSteamPos" );
	mZoneNewLevelLoad = GetUniqueObject( "NewLevelLoadZone" );

	mStages[ "EnterSteamActivateZone" ] = false;
	mStages[ "EnterScaryBarellThrowZone" ] = false;
	mStages[ "EnterObjectiveRestorePowerZone" ] = false;
	mStages[ "EnterObjectiveExaminePlace" ] = false;
	mStages[ "EnterObjectiveNeedCrowbar" ] = false;
	mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] = false;
	mStages[ "DoorUnderFloorOpen" ] = false;
	mStages[ "NeedPassThroughMesh" ] = false;

	AddItem( mCrowbarItem = new Item( GetUniqueObject( "Crowbar" ), Item::Type::Crowbar ));

	mKeypad1 = new Keypad( GetUniqueObject( "Keypad1"), GetUniqueObject( "Keypad1Key0" ), GetUniqueObject( "Keypad1Key1"),
						   GetUniqueObject( "Keypad1Key2"), GetUniqueObject( "Keypad1Key3"), GetUniqueObject( "Keypad1Key4"),
						   GetUniqueObject( "Keypad1Key5"), GetUniqueObject( "Keypad1Key6" ), GetUniqueObject( "Keypad1Key7"),
						   GetUniqueObject( "Keypad1Key8"), GetUniqueObject( "Keypad1Key9"), GetUniqueObject( "Keypad1KeyCancel"), 
						   mKeypad1DoorToUnlock, "3065" );

	mKeypad2 = new Keypad( GetUniqueObject( "Keypad2"), GetUniqueObject( "Keypad2Key0" ), GetUniqueObject( "Keypad2Key1"),
		GetUniqueObject( "Keypad2Key2"), GetUniqueObject( "Keypad2Key3"), GetUniqueObject( "Keypad2Key4"),
		GetUniqueObject( "Keypad2Key5"), GetUniqueObject( "Keypad2Key6" ), GetUniqueObject( "Keypad2Key7"),
		GetUniqueObject( "Keypad2Key8"), GetUniqueObject( "Keypad2Key9"), GetUniqueObject( "Keypad2KeyCancel"), 
		mKeypad2DoorToUnlock, "6497" );

	mKeypad3 = new Keypad( GetUniqueObject( "Keypad3"), GetUniqueObject( "Keypad3Key0" ), GetUniqueObject( "Keypad3Key1"),
		GetUniqueObject( "Keypad3Key2"), GetUniqueObject( "Keypad3Key3"), GetUniqueObject( "Keypad3Key4"),
		GetUniqueObject( "Keypad3Key5"), GetUniqueObject( "Keypad3Key6" ), GetUniqueObject( "Keypad3Key7"),
		GetUniqueObject( "Keypad3Key8"), GetUniqueObject( "Keypad3Key9"), GetUniqueObject( "Keypad3KeyCancel"), 
		mKeypad3DoorToUnlock, "1487" );

	ruSetAudioReverb( 10 );

	pPlayer->SetRockFootsteps();
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

    ruSetAmbientColor( ruVector3( 12.5f / 255.0f, 12.5f / 255.0f, 12.5f / 255.0f ));

    if( mPowerOn ) {
        mLift1->Update();
        mpFan1->DoTurn();
        mpFan2->DoTurn();
    }

	mLift2->Update();

    if( !mStages[ "EnterSteamActivateZone" ] ) {
        if( pPlayer->IsInsideZone( mZoneSteamActivate )) {
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

            mStages[ "EnterSteamActivateZone" ] = true;
        }
    }

	if( !mStages[ "EnterObjectiveRestorePowerZone" ] ) {
		if( pPlayer->IsInsideZone( mZoneObjectiveRestorePower )) {
			pPlayer->SetObjective( mLocalization.GetString( "objectiveRestorePower" ) );
			mStages[ "EnterObjectiveRestorePowerZone" ] = true;
		}
	}

	if( !mStages[ "EnterObjectiveExaminePlace" ] ) {
		if( pPlayer->IsInsideZone( mZoneExaminePlace )) {
			pPlayer->SetObjective( mLocalization.GetString( "objectiveExaminePlace" ) );
			mStages[ "EnterObjectiveExaminePlace" ] = true;
		}
	}

	if( !mStages[ "EnterObjectiveNeedCrowbar" ] ) {
		if( pPlayer->IsInsideZone( mZoneNeedCrowbar )) {
			pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedCrowbar" ) );
			mStages[ "EnterObjectiveNeedCrowbar" ] = true;
		}
	}
	
    if( !mStages[ "EnterScaryBarellThrowZone" ] ) {
        if( pPlayer->IsInsideZone( mZoneScaryBarellThrow )) {
            ruSetNodePosition( mScaryBarell, ruGetNodePosition( mScaryBarellPositionNode ));
            mStages[ "EnterScaryBarellThrowZone" ] = true;
        }
    }

	if( !mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] ) {
		if( pPlayer->IsInsideZone( mZoneNeedCrowbar )) {
			if( pPlayer->GetInventory()->GotAnyItemOfType( Item::Type::Crowbar )) {
				pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedOpenDoorOntoFloor" ) );
				mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] = true;
			}
		}
	}

	if( !mStages[ "NeedPassThroughMesh" ] ) {
		if( pPlayer->IsInsideZone( mZoneObjectiveNeedPassThroughMesh )) {
			pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedPassThroughMesh" ));
			mStages[ "NeedPassThroughMesh" ] = true;
		}
	}
	
	if( !mStages[ "DoorUnderFloorOpen" ] ) {
		if( pPlayer->mNearestPickedNode.IsValid()) {
			if( pPlayer->mNearestPickedNode == mDoorUnderFloor ) {			
				if( pPlayer->GetInventory()->GetItemSelectedForUse() == mCrowbarItem ) {		
					pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->GetLocalization()->GetString( "openDoor" ) );
					if( ruIsKeyHit( pPlayer->mKeyUse )) {
						pPlayer->mInventory.ResetSelectedForUse();
						ruSetNodeRotation( mDoorUnderFloor, ruQuaternion( 0, 0, -110 ));
						mStages[ "DoorUnderFloorOpen" ] = true;
					}
				}
			}
		}
	}

	mKeypad1->Update();
	mKeypad2->Update();
	mKeypad3->Update();

    mpSteamValve->Update();
    mpExtemeSteam->Update();
    mpExtemeSteam->power = 1.0f - mpSteamValve->GetClosedCoeffecient();

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

    if( mpSteamValve->IsDone() ) {
        ruSetNodePosition( mExtremeSteamBlock, ruVector3( 1000, 1000, 1000 ));
    } else {
        if( pPlayer->IsInsideZone( mZoneExtremeSteamHurt )) {
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
                pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->GetLocalization()->GetString( "insertFuse" ) );
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
            pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString("powerUp") );

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
