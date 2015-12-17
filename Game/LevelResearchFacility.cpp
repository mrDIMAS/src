#include "Precompiled.h"

#include "LevelResearchFacility.h"
#include "Player.h"
#include "Door.h"
#include "Utils.h"

LevelResearchFacility::LevelResearchFacility() {
	mpPowerSparks = nullptr;

    mTypeNum = 4;

	mEnemy = nullptr;

    LoadSceneFromFile( "data/maps/release/researchFacility/rf.scene" );
	LoadLocalization( "rf.loc" );

    pPlayer->SetPosition( GetUniqueObject( "PlayerPosition" )->GetPosition() );
	
	AddSound( mSteamHissSound = ruSound::Load3D( "data/sounds/steamhiss.ogg" ));
	mSteamHissSound.SetRolloffFactor( 5 );
	mSteamHissSound.SetReferenceDistance( 4 );
	mSteamHissSound.SetRoomRolloffFactor( 2.5f );

    mLift1 = make_shared<Lift>( GetUniqueObject( "Lift1" ) );
    mLift1->SetControlPanel( GetUniqueObject( "Lift1Screen" ));
    mLift1->SetDestinationPoint( GetUniqueObject( "Lift1Dest" ));
    mLift1->SetSourcePoint( GetUniqueObject( "Lift1Source" ));
    mLift1->SetMotorSound( ruSound::Load3D( "data/sounds/motor_idle.ogg"));
    mLift1->SetBackDoors( GetUniqueObject( "Lift1BackDoor1"), GetUniqueObject( "Lift1BackDoor2" ));
    mLift1->SetFrontDoors( GetUniqueObject( "Lift1FrontDoor1"), GetUniqueObject( "Lift1FrontDoor2" ));
    AddLift( mLift1 );
	
	mLift2 = make_shared<Lift>( GetUniqueObject( "Lift2" ) );
	mLift2->SetControlPanel( GetUniqueObject( "Lift2Screen" ));
	mLift2->SetDestinationPoint( GetUniqueObject( "Lift2Dest" ));
	mLift2->SetSourcePoint( GetUniqueObject( "Lift2Source" ));
	mLift2->SetMotorSound( ruSound::Load3D( "data/sounds/motor_idle.ogg"));
	mLift2->SetBackDoors( GetUniqueObject( "Lift2BackDoor1"), GetUniqueObject( "Lift2BackDoor2" ));
	mLift2->SetFrontDoors( GetUniqueObject( "Lift2FrontDoor1"), GetUniqueObject( "Lift2FrontDoor2" ));
	AddLift( mLift2 );

    mpFan1 = make_shared<Fan>( GetUniqueObject( "Fan" ), 15, ruVector3( 0, 1, 0 ), ruSound::Load3D( "data/sounds/fan.ogg" ));
    mpFan2 = make_shared<Fan>( GetUniqueObject( "Fan2" ), 15, ruVector3( 0, 1, 0 ), ruSound::Load3D( "data/sounds/fan.ogg" ));

	AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note2" ), mLocalization.GetString( "note2Desc" ), mLocalization.GetString( "note2" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note3" ), mLocalization.GetString( "note3Desc" ), mLocalization.GetString( "note3" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note4" ), mLocalization.GetString( "note4Desc" ), mLocalization.GetString( "note4" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note5" ), mLocalization.GetString( "note5Desc" ), mLocalization.GetString( "note5" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note6" ), mLocalization.GetString( "note6Desc" ), mLocalization.GetString( "note6" ) ) );
	AddSheet( new Sheet( GetUniqueObject( "Note7" ), mLocalization.GetString( "note7Desc" ), mLocalization.GetString( "note7" ) ) );

    AddSound( mLeverSound = ruSound::Load3D( "data/sounds/lever.ogg"));

    AddValve( mpSteamValve = make_shared<Valve>( GetUniqueObject( "SteamValve" ), ruVector3( 0, 1, 0 )));
    ruSound steamHis = ruSound::Load3D( "data/sounds/steamhiss_loop.ogg" ) ;
	steamHis.SetRolloffFactor( 5 );
	steamHis.SetReferenceDistance( 4 );
	steamHis.SetRoomRolloffFactor( 2.5f );
    AddSound( steamHis );
    mpExtemeSteam = new SteamStream( GetUniqueObject( "ExtremeSteam"), ruVector3( -0.0015, -0.1, -0.0015 ), ruVector3( 0.0015, -0.45, 0.0015 ), steamHis );

    ruEngine::SetAmbientColor( ruVector3( 0, 0, 0 ));

	mDoorUnderFloor = GetUniqueObject( "DoorUnderFloor" );

    mExtremeSteamBlock = GetUniqueObject( "ExtremeSteamBlock" );
    mZoneExtremeSteamHurt = GetUniqueObject( "ExtremeSteamHurtZone" );

	mThermiteSmall = GetUniqueObject( "ThermiteSmall" );
	mThermiteBig = GetUniqueObject( "ThermiteBig" );

	mRadioHurtZone = GetUniqueObject( "RadioHurtZone" );

	mMeshLock = GetUniqueObject( "MeshLock" );
	mMeshLockAnimation = ruAnimation( 0, 30, 2 );
	mMeshLock->SetAnimation( &mMeshLockAnimation );

	mThermitePlace = GetUniqueObject( "ThermitePlace" );

	mMeshToSewers = GetUniqueObject( "MeshToSewers" );
	mMeshAnimation = ruAnimation( 0, 30, 2 );
	mMeshToSewers->SetAnimation( &mMeshAnimation );

	AddZone( mZoneObjectiveNeedPassThroughMesh = make_shared<Zone>( GetUniqueObject( "ObjectiveNeedPassThroughMesh" )));
	mZoneObjectiveNeedPassThroughMesh->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterNeedPassThroughMeshZone ));

	AddZone( mZoneEnemySpawn = make_shared<Zone>( GetUniqueObject( "ZoneEnemySpawn" )));
	mZoneEnemySpawn->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterSpawnEnemyZone ));

	AddZone( mZoneSteamActivate = make_shared<Zone>( GetUniqueObject( "SteamActivateZone" )));
	mZoneSteamActivate->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterSteamActivateZone ));

	AddZone( mZoneObjectiveRestorePower = make_shared<Zone>( GetUniqueObject( "ObjectiveRestorePower" )));
	mZoneObjectiveRestorePower->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterRestorePowerZone ));

	AddZone( mZoneExaminePlace = make_shared<Zone>( GetUniqueObject( "ObjectiveExaminePlace" )));
	mZoneExaminePlace->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterExaminePlaceZone ));

	AddZone( mZoneRemovePathBlockingMesh = make_shared<Zone>( GetUniqueObject( "ZoneRemovePathBlockingMesh" )));
	mZoneRemovePathBlockingMesh->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterRemovePathBlockingMeshZone ));

	AddZone( mZoneNeedCrowbar = make_shared<Zone>( GetUniqueObject( "ObjectiveNeedCrowbar" )));
	mZoneNeedCrowbar->OnPlayerEnter.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnPlayerEnterNeedCrowbarZone ));

    CreatePowerUpSequence();

    AddSound( mMusic = ruSound::LoadMusic( "data/music/rf.ogg" ));
    mMusic.SetVolume( 0.75f );
	
	AddLadder( make_shared<Ladder>( GetUniqueObject( "LadderBegin" ), GetUniqueObject( "LadderEnd" ), GetUniqueObject( "LadderEnter" ),
		GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder3Begin" ), GetUniqueObject( "Ladder3End" ), GetUniqueObject( "Ladder3Enter" ),
		GetUniqueObject( "Ladder3BeginLeavePoint"), GetUniqueObject( "Ladder3EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder4Begin" ), GetUniqueObject( "Ladder4End" ), GetUniqueObject( "Ladder4Enter" ),
		GetUniqueObject( "Ladder4BeginLeavePoint"), GetUniqueObject( "Ladder4EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder5Begin" ), GetUniqueObject( "Ladder5End" ), GetUniqueObject( "Ladder5Enter" ),
		GetUniqueObject( "Ladder5BeginLeavePoint"), GetUniqueObject( "Ladder5EndLeavePoint")));
	AddLadder( make_shared<Ladder>( GetUniqueObject( "Ladder7Begin" ), GetUniqueObject( "Ladder7End" ), GetUniqueObject( "Ladder7Enter" ),
		GetUniqueObject( "Ladder7BeginLeavePoint"), GetUniqueObject( "Ladder7EndLeavePoint")));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door9" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door10" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door11" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door12" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door13" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door14" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door15" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door16" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door17" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door18" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door19" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door20" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door21" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "Door22" ), 90.0f ));
	AddDoor( make_shared<Door>( GetUniqueObject( "EasterEggDoor" ), 90.0f ));
    AddDoor( mKeypad3DoorToUnlock = make_shared<Door>( GetUniqueObject( "Door4" ), 90.0f ));
    AddDoor( mKeypad1DoorToUnlock = make_shared<Door>( GetUniqueObject( "Door5" ), 90.0f ));
    AddDoor( mKeypad2DoorToUnlock = make_shared<Door>( GetUniqueObject( "Door8" ), 90.0f ));	
	AddDoor( mLabDoorToUnlock = make_shared<Door>( GetUniqueObject( "LabDoor" ), 90 ));
	AddDoor( mColliderDoorToUnlock = make_shared<Door>( GetUniqueObject( "DoorToCollider" ), 90 ));
	AddDoor( mLockedDoor = make_shared<Door>( GetUniqueObject( "LockedDoor" ), 90 ));
	mLockedDoor->SetLocked( true );
	
	mThermiteItemPlace = new ItemPlace( mThermitePlace, Item::Type::AluminumPowder );

	AutoCreateDoorsByNamePattern( "Door?([[:digit:]]+)" );

    mPowerLamp = dynamic_cast<ruPointLight*>( GetUniqueObject( "PowerLamp"));
    mPowerLeverSnd = GetUniqueObject( "PowerLeverSnd");
    mSmallSteamPosition = GetUniqueObject( "RFSteamPos" );
	mZoneNewLevelLoad = GetUniqueObject( "NewLevelLoadZone" );

	mStages[ "EnterSteamActivateZone" ] = false;
	mStages[ "EnterObjectiveRestorePowerZone" ] = false;
	mStages[ "EnterObjectiveExaminePlace" ] = false;
	mStages[ "EnterObjectiveNeedCrowbar" ] = false;
	mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] = false;
	mStages[ "DoorUnderFloorOpen" ] = false;
	mStages[ "NeedPassThroughMesh" ] = false;
	mStages[ "PassedThroughBlockingMesh" ] = false;
	mStages[ "EnemySpawned" ] = false;

	AutoCreateBulletsByNamePattern( "Bullet?([[:digit:]]+)" );

	AddInteractiveObject( Item::GetNameByType( Item::Type::Crowbar ), make_shared<InteractiveObject>( GetUniqueObject( "Crowbar" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveCrowbar ));

	// mCrowbarItem->OnPickup.AddListener( ruDelegate::Bind( this, &LevelResearchFacility::OnCrowbarPickup ));

	AddInteractiveObject( Item::GetNameByType( Item::Type::FerrumOxide ), make_shared<InteractiveObject>( GetUniqueObject( "FerrumOxide" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveFe2O3 ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::AluminumPowder ), make_shared<InteractiveObject>( GetUniqueObject( "AluminumPowder" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveAl ));

	mKeypad1 = new Keypad( GetUniqueObject( "Keypad1"), GetUniqueObject( "Keypad1Key0" ), GetUniqueObject( "Keypad1Key1"),
						   GetUniqueObject( "Keypad1Key2"), GetUniqueObject( "Keypad1Key3"), GetUniqueObject( "Keypad1Key4"),
						   GetUniqueObject( "Keypad1Key5"), GetUniqueObject( "Keypad1Key6" ), GetUniqueObject( "Keypad1Key7"),
						   GetUniqueObject( "Keypad1Key8"), GetUniqueObject( "Keypad1Key9"), GetUniqueObject( "Keypad1KeyCancel"), 
						   mKeypad1DoorToUnlock.get(), "3065" );

	mKeypad2 = new Keypad( GetUniqueObject( "Keypad2"), GetUniqueObject( "Keypad2Key0" ), GetUniqueObject( "Keypad2Key1"),
		GetUniqueObject( "Keypad2Key2"), GetUniqueObject( "Keypad2Key3"), GetUniqueObject( "Keypad2Key4"),
		GetUniqueObject( "Keypad2Key5"), GetUniqueObject( "Keypad2Key6" ), GetUniqueObject( "Keypad2Key7"),
		GetUniqueObject( "Keypad2Key8"), GetUniqueObject( "Keypad2Key9"), GetUniqueObject( "Keypad2KeyCancel"), 
		mKeypad2DoorToUnlock.get(), "6497" );

	mKeypad3 = new Keypad( GetUniqueObject( "Keypad3"), GetUniqueObject( "Keypad3Key0" ), GetUniqueObject( "Keypad3Key1"),
		GetUniqueObject( "Keypad3Key2"), GetUniqueObject( "Keypad3Key3"), GetUniqueObject( "Keypad3Key4"),
		GetUniqueObject( "Keypad3Key5"), GetUniqueObject( "Keypad3Key6" ), GetUniqueObject( "Keypad3Key7"),
		GetUniqueObject( "Keypad3Key8"), GetUniqueObject( "Keypad3Key9"), GetUniqueObject( "Keypad3KeyCancel"), 
		mKeypad3DoorToUnlock.get(), "1487" );

	mLabKeypad = new Keypad( GetUniqueObject( "Keypad4"), GetUniqueObject( "Keypad4Key0" ), GetUniqueObject( "Keypad4Key1"),
		GetUniqueObject( "Keypad4Key2"), GetUniqueObject( "Keypad4Key3"), GetUniqueObject( "Keypad4Key4"),
		GetUniqueObject( "Keypad4Key5"), GetUniqueObject( "Keypad4Key6" ), GetUniqueObject( "Keypad4Key7"),
		GetUniqueObject( "Keypad4Key8"), GetUniqueObject( "Keypad4Key9"), GetUniqueObject( "Keypad4KeyCancel"), 
		mLabDoorToUnlock.get(), "8279" );

	mColliderKeypad = new Keypad( GetUniqueObject( "Keypad5"), GetUniqueObject( "Keypad5Key0" ), GetUniqueObject( "Keypad5Key1"),
		GetUniqueObject( "Keypad5Key2"), GetUniqueObject( "Keypad5Key3"), GetUniqueObject( "Keypad5Key4"),
		GetUniqueObject( "Keypad5Key5"), GetUniqueObject( "Keypad5Key6" ), GetUniqueObject( "Keypad5Key7"),
		GetUniqueObject( "Keypad5Key8"), GetUniqueObject( "Keypad5Key9"), GetUniqueObject( "Keypad5KeyCancel"), 
		mColliderDoorToUnlock.get(), "1598" );


	ruSound::SetAudioReverb( 10 );

	mEnemySpawnPosition = GetUniqueObject( "EnemyPosition" );
		
	mSteamPS = nullptr;

    DoneInitialization();
}

void LevelResearchFacility::CreateEnemy() {
	// create paths
	Path pathStraight; BuildPath( pathStraight, "PathStraight" );
	Path pathCircle; BuildPath( pathCircle, "PathCircle" );
	Path pathRoomA; BuildPath( pathRoomA, "PathRoomA" );
	Path pathRoomB;	BuildPath( pathRoomB, "PathRoomB" );
	Path pathRoomC;	BuildPath( pathRoomC, "PathRoomC" );
	Path pathRoomD;	BuildPath( pathRoomD, "PathRoomD" );
	Path pathToBasement; BuildPath( pathToBasement, "PathToBasement" );

	// add edges
	pathStraight.mVertexList[7]->AddEdge( pathCircle.mVertexList[0] );
	pathStraight.mVertexList[5]->AddEdge( pathToBasement.mVertexList[0] );
	pathStraight.mVertexList[2]->AddEdge( pathRoomA.mVertexList[0] );
	pathStraight.mVertexList[2]->AddEdge( pathRoomB.mVertexList[0] );
	pathStraight.mVertexList[3]->AddEdge( pathRoomC.mVertexList[0] );
	pathStraight.mVertexList[3]->AddEdge( pathRoomD.mVertexList[0] );

	// concatenate paths
	vector<GraphVertex*> allPaths;
	allPaths.insert( allPaths.end(), pathStraight.mVertexList.begin(), pathStraight.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathToBasement.mVertexList.begin(), pathToBasement.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathCircle.mVertexList.begin(), pathCircle.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathRoomA.mVertexList.begin(), pathRoomA.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathRoomB.mVertexList.begin(), pathRoomB.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathRoomC.mVertexList.begin(), pathRoomC.mVertexList.end() );
	allPaths.insert( allPaths.end(), pathRoomD.mVertexList.begin(), pathRoomD.mVertexList.end() );

	// create patrol paths
	vector< GraphVertex* > patrolPoints;

	patrolPoints.push_back( pathToBasement.mVertexList.front() );
	patrolPoints.push_back( pathToBasement.mVertexList.back() );

	patrolPoints.push_back( pathStraight.mVertexList.front() );
	patrolPoints.push_back( pathStraight.mVertexList.back() );

	patrolPoints.push_back( pathCircle.mVertexList.front() );
	patrolPoints.push_back( pathCircle.mVertexList.back() );

	patrolPoints.push_back( pathRoomA.mVertexList.front() );
	patrolPoints.push_back( pathRoomA.mVertexList.back() );

	patrolPoints.push_back( pathRoomB.mVertexList.front() );
	patrolPoints.push_back( pathRoomB.mVertexList.back() );

	patrolPoints.push_back( pathRoomC.mVertexList.front() );
	patrolPoints.push_back( pathRoomC.mVertexList.back() );

	patrolPoints.push_back( pathRoomD.mVertexList.front() );
	patrolPoints.push_back( pathRoomD.mVertexList.back() );

	mEnemy = make_shared<Enemy>( allPaths, patrolPoints );
	mEnemy->SetPosition( mEnemySpawnPosition->GetPosition() );
}

LevelResearchFacility::~LevelResearchFacility() {
	if( mSteamPS ) {
		mSteamPS->Free();
	}
	if( mpPowerSparks ) {
		delete mpPowerSparks;
	}
    delete mpExtemeSteam;
	mBurnSound.Free();
}


void LevelResearchFacility::Show() {
    Level::Show();

    mMusic.Play();
}

void LevelResearchFacility::Hide() {
    Level::Hide();

    mMusic.Pause();
}

void LevelResearchFacility::DoScenario() {
    if( Level::msCurLevelID != LevelName::L3ResearchFacility ) {
        return;
    }

	mMusic.Play();

	mMeshAnimation.Update();
	mMeshLockAnimation.Update();
	
    ruEngine::SetAmbientColor( ruVector3( 10.0f / 255.0f, 10.0f / 255.0f, 10.0f / 255.0f ));
	//ruEngine::SetAmbientColor( ruVector3( 0,0,0 ));
	mLift1->Update();
    if( mPowerOn ) {
        
        mpFan1->DoTurn();
        mpFan2->DoTurn();
    }

	mLift2->Update();

	if( mEnemy ) {
		mEnemy->Think();
	}
	
	if( !mStages[ "DoorUnderFloorOpen" ] ) {
		if( pPlayer->mNearestPickedNode ) {
			if( pPlayer->mNearestPickedNode == mDoorUnderFloor ) {			
				if( pPlayer->GetInventory()->GetItemSelectedForUse() ) {	
					if( pPlayer->GetInventory()->GetItemSelectedForUse()->GetType() == Item::Type::Crowbar ) {
						pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->GetLocalization()->GetString( "openDoor" ) );
						if( ruIsKeyHit( pPlayer->mKeyUse )) {
							pPlayer->mInventory.ResetSelectedForUse();
							mDoorUnderFloor->SetRotation( ruQuaternion( 0, 0, -110 ));
							mStages[ "DoorUnderFloorOpen" ] = true;
						}
					}
				}
			}
		}
	}

	if( pPlayer->IsInsideZone( mRadioHurtZone )) {
		pPlayer->Damage( 0.05, false );
	}

	mKeypad1->Update();
	mKeypad2->Update();
	mKeypad3->Update();
	mLabKeypad->Update();
	mColliderKeypad->Update();

    mpSteamValve->Update();
    mpExtemeSteam->Update();
    mpExtemeSteam->SetPower( 1.0f - mpSteamValve->GetClosedCoeffecient());

    UpdatePowerupSequence();
	UpdateThermiteSequence();

    if( mPowerOn && mpPowerSparks ) {
        mpPowerSparks->Update();

        if( !mpPowerSparks->IsAlive() ) {
            delete mpPowerSparks;

            mpPowerSparks = 0;
        }
    }

    if( mSteamHissSound.IsPlaying() && mSteamPS ) {
        static float steamParticleSize = 0.15f;

        mSteamPS->SetPointSize( steamParticleSize );

        if( steamParticleSize > 0 ) {
            steamParticleSize -= 0.0005f;
        } else {
            mSteamPS->Free();
            mSteamPS = nullptr;
        }
    }

    if( mpSteamValve->IsDone() ) {
        mExtremeSteamBlock->SetPosition( ruVector3( 1000, 1000, 1000 ));
    } else {
        if( pPlayer->IsInsideZone( mZoneExtremeSteamHurt )) {
            pPlayer->Damage( 0.6 );
        }
    }

	if( pPlayer->IsInsideZone( mZoneNewLevelLoad )) {
		Level::Change( L4Sewers );
	}
}

void LevelResearchFacility::UpdateThermiteSequence() {
	if( pPlayer->GetInventory()->GetItemSelectedForUse() ) {
		if( mThermiteItemPlace->IsPickedByPlayer() ) {
			if( ruIsKeyHit( pPlayer->mKeyUse )) {			
				bool placed = mThermiteItemPlace->PlaceItem( pPlayer->mInventory.GetItemSelectedForUse()->GetType() );
				if( placed ) {
					if( mThermiteItemPlace->GetPlaceType() == Item::Type::AluminumPowder ) {
						mThermiteSmall->Show();
						mThermiteItemPlace->SetPlaceType( Item::Type::FerrumOxide );
					} else if( mThermiteItemPlace->GetPlaceType() == Item::Type::FerrumOxide ) {
						mThermiteBig->Show();
						mThermiteItemPlace->SetPlaceType( Item::Type::Lighter );
					} else if( mThermiteItemPlace->GetPlaceType() == Item::Type::Lighter ) {
						mMeshLockAnimation.enabled = true;
						mMeshAnimation.enabled = true;

						mThermiteSmall->Hide();
						mThermiteBig->Hide();

						mBurnSound = ruSound::Load3D( "data/sounds/burn.ogg" );
						mBurnSound.SetPosition( mThermiteSmall->GetPosition() );
						mBurnSound.Play();

						mThermiteItemPlace->SetPlaceType( Item::Type::Unknown );
						
						mThermitePS = ruParticleSystem::Create( 150 );
						mThermitePS->SetPosition( mThermiteSmall->GetPosition() );
						mThermitePS->SetTexture( ruTexture::Request( "data/textures/particles/p1.png"));
						mThermitePS->SetType( ruParticleSystem::Type::Box );
						mThermitePS->SetSpeedDeviation( ruVector3( -0.001, 0.001, -0.001 ), ruVector3( 0.001, 0.009, 0.001 ));
						mThermitePS->SetColorRange( ruVector3( 255, 255, 255 ), ruVector3( 255, 255, 255 ));
						mThermitePS->SetPointSize( 0.045f );
						mThermitePS->SetBoundingBox( ruVector3( -0.2, 0.0, -0.2 ), ruVector3( 0.2, 0.4, 0.2 ));
						mThermitePS->SetParticleThickness( 20.5f );
						mThermitePS->SetAutoResurrection( false );
						mThermitePS->SetLightingEnabled( true );
					}
				}
			}		
			pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->GetLocalization()->GetString( "placeReactive" ) );
		}				
	}
}

void LevelResearchFacility::UpdatePowerupSequence() {
    if( fuseInsertedCount < 3 ) {
        fuseInsertedCount = 0;

        for( int iFuse = 0; iFuse < 3; iFuse++ ) {
            shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
            if( pFuse->GetPlaceType() == Item::Type::Unknown ) {
                fuseInsertedCount++;
            }
        }
    }

    if( pPlayer->GetInventory()->GetItemSelectedForUse() ) {
        for( int iFuse = 0; iFuse < 3; iFuse++ ) {
            shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFuse];
            if( pFuse->IsPickedByPlayer() ) {
                pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->GetLocalization()->GetString( "insertFuse" ) );
            }
        }

        if( ruIsKeyHit( pPlayer->mKeyUse )) {
            for( int iFusePlace = 0; iFusePlace < 3; iFusePlace++ ) {
                shared_ptr<ItemPlace> pFuse = mFusePlaceList[iFusePlace];

                if( pFuse->IsPickedByPlayer() ) {
                    bool placed = pFuse->PlaceItem( pPlayer->mInventory.GetItemSelectedForUse()->GetType() );

                    if( placed ) {
                        fuseModel[iFusePlace]->Show();
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
                mPowerLamp->SetColor( ruVector3( 0, 255, 0 ) );

                mLeverSound.Play();

                mpPowerSparks = new Sparks( mPowerLeverSnd, ruSound::Load3D( "data/sounds/sparks.ogg" ));

                mPowerLeverOnModel->Show();
                mPowerLeverOffModel->Hide();

                mPowerOn = true;
            }
        }
    }
}

void LevelResearchFacility::CreatePowerUpSequence() {
	AddInteractiveObject( Item::GetNameByType( Item::Type::Fuse ), make_shared<InteractiveObject>( GetUniqueObject( "Fuse1" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveFuse ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Fuse ), make_shared<InteractiveObject>( GetUniqueObject( "Fuse2" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveFuse ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Fuse ), make_shared<InteractiveObject>( GetUniqueObject( "Fuse3" )), ruDelegate::Bind( this, &LevelResearchFacility::Proxy_GiveFuse ));

    AddItemPlace( mFusePlaceList[0] = make_shared<ItemPlace>( GetUniqueObject( "FusePlace1" ), Item::Type::Fuse ));
    AddItemPlace( mFusePlaceList[1] = make_shared<ItemPlace>( GetUniqueObject( "FusePlace2" ), Item::Type::Fuse ));
    AddItemPlace( mFusePlaceList[2] = make_shared<ItemPlace>( GetUniqueObject( "FusePlace3" ), Item::Type::Fuse ));

    fuseModel[0] = GetUniqueObject( "FuseModel1" );
    fuseModel[1] = GetUniqueObject( "FuseModel2" );
    fuseModel[2] = GetUniqueObject( "FuseModel3" );

    mPowerLeverOnModel = GetUniqueObject( "PowerSwitchOnModel" );
    mPowerLeverOffModel = GetUniqueObject( "PowerSwitchOffModel" );
    powerLever = GetUniqueObject( "PowerLever" );

    fuseInsertedCount = 0;

    mPowerOn = false;
}

void LevelResearchFacility::OnDeserialize( SaveFile & in ) {
	if( in.ReadBoolean() ) {
		CreateEnemy();
		mEnemy->SetPosition( in.ReadVector3() );
	}
	DeserializeAnimation( in, mMeshAnimation );
	DeserializeAnimation( in, mMeshLockAnimation );
}

void LevelResearchFacility::OnSerialize( SaveFile & out ) {
	out.WriteBoolean( mEnemy != nullptr );
	if( mEnemy ) {
		out.WriteVector3( mEnemy->GetBody()->GetPosition());
	}
	SerializeAnimation( out, mMeshAnimation );
	SerializeAnimation( out, mMeshLockAnimation );
}

void LevelResearchFacility::OnCrowbarPickup()
{
	if( !mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] ) {
		pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedOpenDoorOntoFloor" ) );
		mStages[ "EnterObjectiveNeedOpenDoorOntoFloor" ] = true;						
	}
}

void LevelResearchFacility::OnPlayerEnterNeedCrowbarZone()
{
	if( !mStages[ "EnterObjectiveNeedCrowbar" ] ) {
		pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedCrowbar" ) );
		mStages[ "EnterObjectiveNeedCrowbar" ] = true;			
	}
}

void LevelResearchFacility::OnPlayerEnterRemovePathBlockingMeshZone()
{
	if( !mStages[ "PassedThroughBlockingMesh" ] ) {
		mLockedDoor->SetLocked( false );
		mStages[ "PassedThroughBlockingMesh" ] = true;			
	}
}

void LevelResearchFacility::OnPlayerEnterExaminePlaceZone()
{
	if( !mStages[ "EnterObjectiveExaminePlace" ] ) {
		pPlayer->SetObjective( mLocalization.GetString( "objectiveExaminePlace" ) );
		mStages[ "EnterObjectiveExaminePlace" ] = true;			
	}
}

void LevelResearchFacility::OnPlayerEnterRestorePowerZone()
{
	if( !mStages[ "EnterObjectiveRestorePowerZone" ] ) {
		pPlayer->SetObjective( mLocalization.GetString( "objectiveRestorePower" ) );
		mStages[ "EnterObjectiveRestorePowerZone" ] = true;
	}
}

void LevelResearchFacility::OnPlayerEnterSteamActivateZone() {
	if( !mStages[ "EnterSteamActivateZone" ] ) {
		mSteamPS = ruParticleSystem::Create( 35 );
		mSteamPS->SetPosition( mSmallSteamPosition->GetPosition());
		mSteamPS->SetTexture( ruTexture::Request( "data/textures/particles/p1.png" ));
		mSteamPS->SetType( ruParticleSystem::Type::Stream );
		mSteamPS->SetSpeedDeviation( ruVector3( -0.0015, 0.08, -0.0015 ), ruVector3( 0.0015, 0.2, 0.0015 ));
		mSteamPS->SetBoundingRadius( 0.4f );
		mSteamPS->SetColorRange( ruVector3( 255, 255, 255 ), ruVector3( 255, 255, 255 ));
		mSteamPS->SetPointSize( 0.15f );
		mSteamPS->SetParticleThickness( 1.5f );
		mSteamPS->SetLightingEnabled( true );

		mSteamHissSound.Attach( mSteamPS );
		mSteamHissSound.Play();
		mStages[ "EnterSteamActivateZone" ] = true;	
	}
}

void LevelResearchFacility::OnPlayerEnterSpawnEnemyZone()
{
	if( !mStages[ "EnemySpawned" ] ) {
		CreateEnemy();
		mStages[ "EnemySpawned" ] = true;			
	}
}

void LevelResearchFacility::OnPlayerEnterNeedPassThroughMeshZone()
{
	if( !mStages[ "NeedPassThroughMesh" ] ) {
		pPlayer->SetObjective( mLocalization.GetString( "objectiveNeedPassThroughMesh" ));
		mStages[ "NeedPassThroughMesh" ] = true;			
	}
}
