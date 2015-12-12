#include "Precompiled.h"

#include "LevelMine.h"
#include "Player.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine() {
    mTypeNum = 3;

    LoadLocalization( "mine.loc" );

    LoadSceneFromFile( "data/maps/release/mine/mine.scene");

    pPlayer->SetPosition( GetUniqueObject( "PlayerPosition" ).GetPosition() );

    ruVector3 placePos = GetUniqueObject( "PlayerPosition" ).GetPosition();
    ruVector3 playerPos = pPlayer->GetCurrentPosition();

    pPlayer->SetObjective( mLocalization.GetString( "objective1" ));

    AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note2" ), mLocalization.GetString( "note2Desc" ), mLocalization.GetString( "note2" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note3" ), mLocalization.GetString( "note3Desc" ), mLocalization.GetString( "note3" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note4" ), mLocalization.GetString( "note4Desc" ), mLocalization.GetString( "note4" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note5" ), mLocalization.GetString( "note5Desc" ), mLocalization.GetString( "note5" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note6" ), mLocalization.GetString( "note6Desc" ), mLocalization.GetString( "note6" ) ) );

    mStoneFallZone = GetUniqueObject( "StoneFallZone" );

    mNewLevelZone = GetUniqueObject( "NewLevel" );

    ruSetAudioReverb( 10 );

    AddSound( mMusic = ruSound::LoadMusic( "data/music/chapter2.ogg" ));

    mConcreteWall = GetUniqueObject( "ConcreteWall" );
    mDeathZone = GetUniqueObject( "DeadZone" );
    mDetonator = GetUniqueObject( "Detonator" );

    AddSound( mAlertSound = ruSound::Load3D( "data/sounds/alert.ogg" ));
    mAlertSound.Attach( mDetonator );

    AddSound( mExplosionSound = ruSound::Load3D( "data/sounds/blast.ogg" ));
    mExplosionSound.SetReferenceDistance( 10 );

    mDetonatorActivated = 0;

    mExplosionFlashAnimator = 0;

	//AddItem( new Item( GetUniqueObject( "Pistol" ), Item::Type::Pistol ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Pistol ), make_shared<InteractiveObject>( GetUniqueObject( "Pistol" )), ruDelegate::Bind( this, &LevelMine::Proxy_GivePistol ));

	AutoCreateBulletsByNamePattern( "Bullet?([[:digit:]]+)" );

    // Create detonator places
    AddItemPlace( mDetonatorPlace[0] = make_shared<ItemPlace>( GetUniqueObject( "DetonatorPlace1" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[1] = make_shared<ItemPlace>( GetUniqueObject( "DetonatorPlace2" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[2] = make_shared<ItemPlace>( GetUniqueObject( "DetonatorPlace3" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[3] = make_shared<ItemPlace>( GetUniqueObject( "DetonatorPlace4" ), Item::Type::Explosives ));

    

    mWireModels[0] = GetUniqueObject( "WireModel1" );
    mWireModels[1] = GetUniqueObject( "WireModel2" );
    mWireModels[2] = GetUniqueObject( "WireModel3" );
    mWireModels[3] = GetUniqueObject( "WireModel4" );

    mDetonatorModels[0] = GetUniqueObject( "DetonatorModel1" );
    mDetonatorModels[1] = GetUniqueObject( "DetonatorModel2" );
    mDetonatorModels[2] = GetUniqueObject( "DetonatorModel3" );
    mDetonatorModels[3] = GetUniqueObject( "DetonatorModel4" );

    mExplosivesModels[0] = GetUniqueObject( "ExplosivesModel1" );
    mExplosivesModels[1] = GetUniqueObject( "ExplosivesModel2" );
    mExplosivesModels[2] = GetUniqueObject( "ExplosivesModel3" );
    mExplosivesModels[3] = GetUniqueObject( "ExplosivesModel4" );

    mFindItemsZone = GetUniqueObject( "FindItemsZone" );

    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/mine/ambientmine1.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/mine/ambientmine2.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/mine/ambientmine3.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/mine/ambientmine4.ogg" ));
    AddAmbientSound( ruSound::Load3D( "data/sounds/ambient/mine/ambientmine5.ogg" ));

    mExplosionTimer = ruCreateTimer();

    mBeepSoundTimer = ruCreateTimer();
    mBeepSoundTiming = 1.0f;

    CreateItems();

    mReadyExplosivesCount = 0;

    AddLadder( make_shared<Ladder>( GetUniqueObject( "LadderBegin" ), GetUniqueObject( "LadderEnd" ), GetUniqueObject( "LadderEnter" ),
                           GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint")));
    AddDoor( make_shared<Door>( GetUniqueObject( "Door1" ), 90 ));

    mMusic.Play();

    mStages[ "EnterRockFallZoneWallExp" ] = false;
    mStages[ "EnterScreamerDone" ] = false;
    mStages[ "EnterScreamer2Done" ] = false;
    mStages[ "ConcreteWallExp" ] = false;
    mStages[ "FindObjectObjectiveSet" ] = false;
    mStages[ "FoundObjectsForExplosion" ] = false;

    // create paths
    Path path;
    BuildPath( path, "Path" );
    Path pathOnUpperLevel;
    BuildPath( pathOnUpperLevel, "PathOnUpperLevel" );
    Path pathUpperRight;
    BuildPath( pathUpperRight, "PathUpperRight" );
    Path pathUpperLeft;
    BuildPath( pathUpperLeft, "PathUpperLeft" );
    Path pathToRoom;
    BuildPath( pathToRoom, "PathToRoom" );
    Path pathUpperRightTwo;
    BuildPath( pathUpperRightTwo, "PathUpperRightTwo" );

    // cross-path edges
    path.mVertexList[17]->AddEdge( pathOnUpperLevel.mVertexList[0] );
    pathOnUpperLevel.mVertexList[6]->AddEdge( pathUpperRight.mVertexList[0] );
    pathOnUpperLevel.mVertexList[8]->AddEdge( pathUpperLeft.mVertexList[0] );
    path.mVertexList[18]->AddEdge( pathToRoom.mVertexList[0] );
    pathOnUpperLevel.mVertexList[12]->AddEdge( pathUpperRightTwo.mVertexList[0] );

    // concatenate all paths
    vector<GraphVertex*> allPaths;
    allPaths.insert( allPaths.end(), path.mVertexList.begin(), path.mVertexList.end() );
    allPaths.insert( allPaths.end(), pathOnUpperLevel.mVertexList.begin(), pathOnUpperLevel.mVertexList.end() );
    allPaths.insert( allPaths.end(), pathUpperRight.mVertexList.begin(), pathUpperRight.mVertexList.end() );
    allPaths.insert( allPaths.end(), pathUpperLeft.mVertexList.begin(), pathUpperLeft.mVertexList.end() );
    allPaths.insert( allPaths.end(), pathToRoom.mVertexList.begin(), pathToRoom.mVertexList.end() );
    allPaths.insert( allPaths.end(), pathUpperRightTwo.mVertexList.begin(), pathUpperRightTwo.mVertexList.end() );

    vector< GraphVertex* > patrolPoints;
    patrolPoints.push_back( path.mVertexList.front() );
    patrolPoints.push_back( path.mVertexList.back() );

    patrolPoints.push_back( pathOnUpperLevel.mVertexList.front() );
    patrolPoints.push_back( pathOnUpperLevel.mVertexList.back() );

    patrolPoints.push_back( pathUpperRight.mVertexList.front() );
    patrolPoints.push_back( pathUpperRight.mVertexList.back() );

    patrolPoints.push_back( pathUpperLeft.mVertexList.front() );
    patrolPoints.push_back( pathUpperLeft.mVertexList.back() );

    patrolPoints.push_back( pathUpperRightTwo.mVertexList.front() );
    patrolPoints.push_back( pathUpperRightTwo.mVertexList.back() );

    patrolPoints.push_back( pathToRoom.mVertexList.front() );
    patrolPoints.push_back( pathToRoom.mVertexList.back() );

    mEnemy = new Enemy( allPaths, patrolPoints );
    mEnemy->SetPosition( GetUniqueObject( "EnemyPosition" ).GetPosition());

    mRock[0] = GetUniqueObject( "Rock1" );
    mRock[1] = GetUniqueObject( "Rock2" );
    mRock[2] = GetUniqueObject( "Rock3" );

    mExplosivesDummy[0] = GetUniqueObject( "ExplosivesModel5" );
    mExplosivesDummy[1] = GetUniqueObject( "ExplosivesModel6" );
    mExplosivesDummy[2] = GetUniqueObject( "ExplosivesModel7" );
    mExplosivesDummy[3] = GetUniqueObject( "ExplosivesModel8" );

    mRockPosition[0] = GetUniqueObject( "Rock1Pos" ).GetPosition();
    mRockPosition[1] = GetUniqueObject( "Rock2Pos" ).GetPosition();
    mRockPosition[2] = GetUniqueObject( "Rock3Pos" ).GetPosition();

    mExplosionFlashPosition = GetUniqueObject( "ExplosionFlash" );

    DoneInitialization();
}

LevelMine::~LevelMine() {

}

void LevelMine::Show() {
    Level::Show();
}

void LevelMine::Hide() {
    Level::Hide();

    mMusic.Pause();
}

void LevelMine::DoScenario() {
    if( Level::msCurLevelID != LevelName::L2Mine ) {
        return;
    }

	mMusic.Play();

    mEnemy->Think();

	ruEngine::SetAmbientColor( ruVector3( 8.0f / 255.0f, 8.0f / 255.0f, 8.0f / 255.0f ));

    PlayAmbientSounds();

    if( !mStages[ "EnterRockFallZoneWallExp" ] ) {
        if( pPlayer->IsInsideZone( mStoneFallZone )) {
            ruSceneNode::FindByName( "StoneFall" ).Unfreeze();

            mStages[ "EnterRockFallZoneWallExp" ] = true;
        }
    }

    if( !mStages[ "FindObjectObjectiveSet" ] ) {
        if( !mStages[ "FoundObjectsForExplosion" ] ) {
            if( pPlayer->IsInsideZone( mFindItemsZone )) {
                pPlayer->SetObjective( mLocalization.GetString( "objective2" ) );

                mStages[ "FindObjectObjectiveSet" ] = true;
            }
        } else {
            mStages[ "FindObjectObjectiveSet" ] = true;
        }
    }

    if( mExplosionFlashAnimator ) {
        mExplosionFlashAnimator->Update();
    }

    if( !mStages[ "ConcreteWallExp" ] ) {
        if( pPlayer->mNearestPickedNode == mDetonator ) {
            pPlayer->SetActionText( mLocalization.GetString( "detonator" ));

            if( ruIsKeyHit( pPlayer->mKeyUse ) && mReadyExplosivesCount >= 4 && !mDetonatorActivated ) {
                mDetonatorActivated = 1;

                ruRestartTimer( mExplosionTimer );
            }
        }

        if( mDetonatorActivated ) {
            if( ruGetElapsedTimeInSeconds( mExplosionTimer ) >= 10.0f ) {
                mDetonatorActivated = 0;

                mExplosionSound.Play();

                mExplosionSound.SetPosition( mConcreteWall.GetPosition() );

                mStages[ "ConcreteWallExp" ] = true;

                mConcreteWall.SetPosition( ruVector3( 10000, 10000, 10000 ));

                CleanUpExplodeArea();

                ruVector3 vec = ( mConcreteWall.GetPosition() - pPlayer->GetCurrentPosition() ).Normalize() * 20;

                for( int iRock = 0; iRock < 3; iRock++ ) {
                    mRock[iRock].SetPosition( mRockPosition[iRock] );
                }

                mExplosionFlashLight = ruLight::Create( ruLight::Type::Point );
                mExplosionFlashLight.Attach( mExplosionFlashPosition );
                mExplosionFlashLight.SetColor( ruVector3( 255, 200, 160 ));
                mExplosionFlashAnimator = new LightAnimator( mExplosionFlashLight, 0.25, 30, 1.1 );
                mExplosionFlashAnimator->SetAnimationType( LightAnimator::AnimationType::Off );

                // dust
                ruParticleSystemProperties dustProps;
                dustProps.texture = ruGetTexture( "data/textures/particles/p1.png" );
                dustProps.type = PS_BOX;
                dustProps.useLighting = true;
                dustProps.autoResurrectDeadParticles = false;
                dustProps.pointSize = 1.1f;
                dustProps.speedDeviationMin = ruVector3( -0.0005, 0.0, -0.0005 );
                dustProps.speedDeviationMax = ruVector3(  0.0005, 0.005,  0.0005 );
                dustProps.boundingBoxMin = ruVector3( -2, 0, -4 );
                dustProps.boundingBoxMax = ruVector3( 2, 4, 4 );
                dustProps.colorBegin = ruVector3( 20, 20, 20 );
                dustProps.colorEnd = ruVector3( 40, 40, 40 );

                mExplosionDustParticleSystem = ruCreateParticleSystem( 400, dustProps );
                mExplosionDustParticleSystem.SetPosition( mExplosivesDummy[0].GetPosition() - ruVector3( 0, 2.5, 0 ));

                if( pPlayer->IsInsideZone( mDeathZone )) {
                    pPlayer->Damage( 1000 );
                    pPlayer->Move( vec, 1 );
                }
            }

            if( ruGetElapsedTimeInSeconds( mBeepSoundTimer ) > mBeepSoundTiming ) { // every 1 sec
                mBeepSoundTiming -= 0.05f;

                ruRestartTimer( mBeepSoundTimer );

                mAlertSound.Play( false );
            }
        }
    }

    if( pPlayer->IsInsideZone( mNewLevelZone )) {
        Level::Change( LevelName::L3ResearchFacility );

        return;
    }

    UpdateExplodeSequence();
}

void LevelMine::UpdateExplodeSequence() {
    if( mReadyExplosivesCount < 4 ) {
        mReadyExplosivesCount = 0;

        for( int i = 0; i < 4; i++ ) {
            shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

            if( dp->GetPlaceType() == Item::Type::Unknown ) {
                mReadyExplosivesCount++;
            }

            if( mReadyExplosivesCount >= 4 ) {
                pPlayer->SetObjective( mLocalization.GetString( "objective4" ) );
            }
        }
    }

    static int totalNeededObjects = 0;
    if( totalNeededObjects < 12 ) {
        totalNeededObjects = 0;
        totalNeededObjects += pPlayer->mInventory.GetItemCount( Item::Type::Wires );
        totalNeededObjects += pPlayer->mInventory.GetItemCount( Item::Type::Explosives );
        totalNeededObjects += pPlayer->mInventory.GetItemCount( Item::Type::Detonator );
        if( totalNeededObjects >= 12 ) {
            mStages[ "FindObjectObjectiveSet" ] = true;
            pPlayer->SetObjective( mLocalization.GetString( "objective3" ) );
        }
    }

    if( pPlayer->mInventory.GetItemSelectedForUse() ) {
        for( int i = 0; i < 4; i++ ) {
            shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

            if( dp->IsPickedByPlayer() ) {
                pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString( "putItem") );
            }
        }

        if( ruIsKeyHit( pPlayer->mKeyUse )) {
            for( int i = 0; i < 4; i++ ) {
                shared_ptr<ItemPlace> dp = mDetonatorPlace[i];

                if( dp->IsPickedByPlayer() ) {
                    bool placed = dp->PlaceItem( pPlayer->mInventory.GetItemSelectedForUse()->GetType() );

                    if( placed ) {
                        // 1st: Explosives
                        // 2nd: Detonator
                        // 3rd: Wires
                        // 4th: Ready to explode
                        if( dp->GetPlaceType() == Item::Type::Explosives ) {
                            mExplosivesModels[i].Show();
                            dp->SetPlaceType( Item::Type::Detonator );
                        } else if( dp->GetPlaceType() == Item::Type::Detonator ) {
                            mDetonatorModels[i].Show();
                            dp->SetPlaceType( Item::Type::Wires );
                        } else if( dp->GetPlaceType() == Item::Type::Wires ) {
                            mWireModels[i].Show();
                            dp->SetPlaceType( Item::Type::Unknown );
                        }
                    }
                }
            }
        }
    }
}

void LevelMine::CleanUpExplodeArea() {
    for( int i = 0; i < 4; i++ ) {
        mDetonatorPlace[i]->mObject.SetPosition( ruVector3( 1000, 1000, 1000 ));
        mWireModels[i].Hide();
        mExplosivesModels[i].Hide();
        mDetonatorModels[i].Hide();
        mExplosivesDummy[i].Hide();
    }
}

void LevelMine::CreateItems() {
	// Create explosives
	AddInteractiveObject( Item::GetNameByType( Item::Type::Explosives ), make_shared<InteractiveObject>( GetUniqueObject( "Explosives1" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveExplosives ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Explosives ), make_shared<InteractiveObject>( GetUniqueObject( "Explosives2" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveExplosives ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Explosives ), make_shared<InteractiveObject>( GetUniqueObject( "Explosives3" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveExplosives ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Explosives ), make_shared<InteractiveObject>( GetUniqueObject( "Explosives4" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveExplosives ));

    // Create detonators
	AddInteractiveObject( Item::GetNameByType( Item::Type::Detonator ), make_shared<InteractiveObject>( GetUniqueObject( "Detonator1" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveDetonator ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Detonator ), make_shared<InteractiveObject>( GetUniqueObject( "Detonator2" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveDetonator ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Detonator ), make_shared<InteractiveObject>( GetUniqueObject( "Detonator3" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveDetonator ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Detonator ), make_shared<InteractiveObject>( GetUniqueObject( "Detonator4" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveDetonator ));

    // Create wires
	AddInteractiveObject( Item::GetNameByType( Item::Type::Wires ), make_shared<InteractiveObject>( GetUniqueObject( "Wire1" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveWires ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Wires ), make_shared<InteractiveObject>( GetUniqueObject( "Wire2" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveWires ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Wires ), make_shared<InteractiveObject>( GetUniqueObject( "Wire3" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveWires ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::Wires ), make_shared<InteractiveObject>( GetUniqueObject( "Wire4" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveWires ));

	AddInteractiveObject( Item::GetNameByType( Item::Type::FuelCanister ), make_shared<InteractiveObject>( GetUniqueObject( "Fuel1" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveFuel ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::FuelCanister ), make_shared<InteractiveObject>( GetUniqueObject( "Fuel2" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveFuel ));
	AddInteractiveObject( Item::GetNameByType( Item::Type::FuelCanister ), make_shared<InteractiveObject>( GetUniqueObject( "Fuel3" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveFuel ));

	AddInteractiveObject( Item::GetNameByType( Item::Type::Syringe ), make_shared<InteractiveObject>( GetUniqueObject( "Syringe" )), ruDelegate::Bind( this, &LevelMine::Proxy_GiveSyringe ));
}

void LevelMine::OnDeserialize( SaveFile & in ) {
    in.ReadBoolean( mDetonatorActivated );
    in.ReadFloat( mBeepSoundTiming );
    mEnemy->Deserialize( in );
}

void LevelMine::OnSerialize( SaveFile & out ) {
    out.WriteBoolean( mDetonatorActivated );
    out.WriteFloat( mBeepSoundTiming );
    mEnemy->Serialize( out );
}
