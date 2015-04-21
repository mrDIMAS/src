#include "Precompiled.h"

#include "LevelMine.h"
#include "Player.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine() {
    mTypeNum = 3;

    LoadLocalization( "mine.loc" );

    LoadSceneFromFile( "data/maps/release/mine/mine.scene");

    pPlayer->SetPosition( ruGetNodePosition( GetUniqueObject( "PlayerPosition" )));

    ruVector3 placePos = ruGetNodePosition( GetUniqueObject( "PlayerPosition" ));
    ruVector3 playerPos = pPlayer->GetCurrentPosition();

    pPlayer->SetObjective( mLocalization.GetString( "objective1" ));

    AddSheet( new Sheet( GetUniqueObject( "Note1" ), mLocalization.GetString( "note1Desc" ), mLocalization.GetString( "note1" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note2" ), mLocalization.GetString( "note2Desc" ), mLocalization.GetString( "note2" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note3" ), mLocalization.GetString( "note3Desc" ), mLocalization.GetString( "note3" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note4" ), mLocalization.GetString( "note4Desc" ), mLocalization.GetString( "note4" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note5" ), mLocalization.GetString( "note5Desc" ), mLocalization.GetString( "note5" ) ) );
    AddSheet( new Sheet( GetUniqueObject( "Note6" ), mLocalization.GetString( "note6Desc" ), mLocalization.GetString( "note6" ) ) );

    mScreamerZone = GetUniqueObject( "Screamer1" );
    mScreamerZone2 = GetUniqueObject( "Screamer2" );
    mStoneFallZone = GetUniqueObject( "StoneFallZone" );

    mNewLevelZone = GetUniqueObject( "NewLevel" );

    ruSetAudioReverb( 10 );

    AddSound( mMusic = ruLoadMusic( "data/music/chapter2.ogg" ));

    mConcreteWall = GetUniqueObject( "ConcreteWall" );
    mDeathZone = GetUniqueObject( "DeadZone" );
    mDetonator = GetUniqueObject( "Detonator" );

    AddSound( mAlertSound = ruLoadSound3D( "data/sounds/alert.ogg" ));
    ruAttachSound( mAlertSound, mDetonator );

    AddSound( mExplosionSound = ruLoadSound3D( "data/sounds/blast.ogg" ));
    ruSetSoundReferenceDistance( mExplosionSound, 10 );

    mDetonatorActivated = 0;

    mExplosionFlashAnimator = 0;

	AddItem( new Item( GetUniqueObject( "Pistol" ), Item::Type::Pistol ));

	AutoCreateBulletsByNamePattern( "Bullet?([[:digit:]]+)" );

    // Create detonator places
    AddItemPlace( mDetonatorPlace[0] = new ItemPlace( GetUniqueObject( "DetonatorPlace1" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[1] = new ItemPlace( GetUniqueObject( "DetonatorPlace2" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[2] = new ItemPlace( GetUniqueObject( "DetonatorPlace3" ), Item::Type::Explosives ));
    AddItemPlace( mDetonatorPlace[3] = new ItemPlace( GetUniqueObject( "DetonatorPlace4" ), Item::Type::Explosives ));

    // Create explosives
    AddItem( mExplosives[ 0 ] = new Item( GetUniqueObject( "Explosives1" ), Item::Type::Explosives ));
    AddItem( mExplosives[ 1 ] = new Item( GetUniqueObject( "Explosives2" ), Item::Type::Explosives ));
    AddItem( mExplosives[ 2 ] = new Item( GetUniqueObject( "Explosives3" ), Item::Type::Explosives ));
    AddItem( mExplosives[ 3 ] = new Item( GetUniqueObject( "Explosives4" ), Item::Type::Explosives ));

    // Create detonators
    AddItem( mDetonators[ 0 ] = new Item( GetUniqueObject( "Detonator1" ), Item::Type::Detonator ));
    AddItem( mDetonators[ 1 ] = new Item( GetUniqueObject( "Detonator2" ), Item::Type::Detonator ));
    AddItem( mDetonators[ 2 ] = new Item( GetUniqueObject( "Detonator3" ), Item::Type::Detonator ));
    AddItem( mDetonators[ 3 ] = new Item( GetUniqueObject( "Detonator4" ), Item::Type::Detonator ));

    // Create wires
    AddItem( mWires[ 0 ] = new Item( GetUniqueObject( "Wire1" ), Item::Type::Wires ));
    AddItem( mWires[ 1 ] = new Item( GetUniqueObject( "Wire2" ), Item::Type::Wires ));
    AddItem( mWires[ 2 ] = new Item( GetUniqueObject( "Wire3" ), Item::Type::Wires ));
    AddItem( mWires[ 3 ] = new Item( GetUniqueObject( "Wire4" ), Item::Type::Wires ));

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

    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine1.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine2.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine3.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine4.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine5.ogg" ));

    mExplosionTimer = ruCreateTimer();

    mBeepSoundTimer = ruCreateTimer();
    mBeepSoundTiming = 1.0f;

    CreateItems();

    mReadyExplosivesCount = 0;

    pPlayer->SetFootsteps( FootstepsType::Rock );

    AddLadder( new Ladder( GetUniqueObject( "LadderBegin" ), GetUniqueObject( "LadderEnd" ), GetUniqueObject( "LadderEnter" ),
                           GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint")));
    AddDoor( new Door( GetUniqueObject( "Door1" ), 90 ));

    ruPlaySound( mMusic );

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
    patrolPoints.push_back( path.mVertexList[0] );
    patrolPoints.push_back( path.mVertexList[ path.mVertexList.size() - 1 ] );
    patrolPoints.push_back( pathOnUpperLevel.mVertexList[0] );
    patrolPoints.push_back( pathOnUpperLevel.mVertexList[ pathOnUpperLevel.mVertexList.size() - 1 ] );
    patrolPoints.push_back( pathUpperRight.mVertexList[0] );
    patrolPoints.push_back( pathUpperRight.mVertexList[ pathUpperRight.mVertexList.size() - 1 ] );
    patrolPoints.push_back( pathUpperLeft.mVertexList[0] );
    patrolPoints.push_back( pathUpperLeft.mVertexList[ pathUpperLeft.mVertexList.size() - 1 ] );
    patrolPoints.push_back( pathUpperRightTwo.mVertexList[0] );
    patrolPoints.push_back( pathUpperRightTwo.mVertexList[ pathUpperRightTwo.mVertexList.size() - 1 ] );
    patrolPoints.push_back( pathToRoom.mVertexList[0] );
    patrolPoints.push_back( pathToRoom.mVertexList[ pathToRoom.mVertexList.size() - 1 ] );

    mEnemy = new Enemy( "data/models/ripper/ripper.scene", allPaths, patrolPoints );
    mEnemy->SetPosition( ruGetNodePosition( GetUniqueObject( "EnemyPosition" )));

    mRock[0] = GetUniqueObject( "Rock1" );
    mRock[1] = GetUniqueObject( "Rock2" );
    mRock[2] = GetUniqueObject( "Rock3" );

    mExplosivesDummy[0] = GetUniqueObject( "ExplosivesModel5" );
    mExplosivesDummy[1] = GetUniqueObject( "ExplosivesModel6" );
    mExplosivesDummy[2] = GetUniqueObject( "ExplosivesModel7" );
    mExplosivesDummy[3] = GetUniqueObject( "ExplosivesModel8" );

    mRockPosition[0] = ruGetNodePosition( GetUniqueObject( "Rock1Pos" ));
    mRockPosition[1] = ruGetNodePosition( GetUniqueObject( "Rock2Pos" ));
    mRockPosition[2] = ruGetNodePosition( GetUniqueObject( "Rock3Pos" ));

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

    ruPauseSound( mMusic );
}

void LevelMine::DoScenario() {
    if( Level::msCurLevelID != LevelName::L2Mine ) {
        return;
    }

	ruPlaySound( mMusic );

    mEnemy->Think();

    ruSetAmbientColor( ruVector3( 0.08, 0.08, 0.08 ));

    PlayAmbientSounds();

    if( !mStages[ "EnterRockFallZoneWallExp" ] ) {
        if( pPlayer->IsInsideZone( mStoneFallZone )) {
            ruUnfreeze( ruFindByName( "StoneFall" ) );

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

                ruPlaySound( mExplosionSound );

                ruSetSoundPosition( mExplosionSound, ruGetNodePosition( mConcreteWall ) );

                mStages[ "ConcreteWallExp" ] = true;

                ruSetNodePosition( mConcreteWall, ruVector3( 10000, 10000, 10000 ));

                CleanUpExplodeArea();

                ruVector3 vec = ( ruGetNodePosition( mConcreteWall ) - pPlayer->GetCurrentPosition() ).Normalize() * 20;

                for( int iRock = 0; iRock < 3; iRock++ ) {
                    ruSetNodePosition( mRock[iRock], mRockPosition[iRock] );
                }

                mExplosionFlashLight = ruCreateLight();
                ruAttachNode( mExplosionFlashLight, mExplosionFlashPosition );
                ruSetLightColor( mExplosionFlashLight, ruVector3( 255, 200, 160 ));
                mExplosionFlashAnimator = new LightAnimator( mExplosionFlashLight, 0.25, 30, 1.1 );
                mExplosionFlashAnimator->SetAnimationType( LightAnimator::AnimationType::Off );

                // dust
                ruParticleSystemProperties dustProps;
                dustProps.texture = ruGetTexture( "data/textures/particles/p1.png" );
                dustProps.type = PS_BOX;
                dustProps.useLighting = false;
                dustProps.autoResurrectDeadParticles = false;
                dustProps.pointSize = 0.1f;
                dustProps.speedDeviationMin = ruVector3( -0.0005, 0.0, -0.0005 );
                dustProps.speedDeviationMax = ruVector3(  0.0005, 0.005,  0.0005 );
                dustProps.boundingBoxMin = ruVector3( -2, 0, -4 );
                dustProps.boundingBoxMax = ruVector3( 2, 4, 4 );
                dustProps.colorBegin = ruVector3( 20, 20, 20 );
                dustProps.colorEnd = ruVector3( 40, 40, 40 );

                mExplosionDustParticleSystem = ruCreateParticleSystem( 400, dustProps );
                ruSetNodePosition( mExplosionDustParticleSystem, ruGetNodePosition( mExplosivesDummy[0] ) - ruVector3( 0, 2.5, 0 ));

                if( pPlayer->IsInsideZone( mDeathZone )) {
                    pPlayer->Damage( 1000 );
                    pPlayer->Move( vec, 1 );
                }
            }

            if( ruGetElapsedTimeInSeconds( mBeepSoundTimer ) > mBeepSoundTiming ) { // every 1 sec
                mBeepSoundTiming -= 0.05f;

                ruRestartTimer( mBeepSoundTimer );

                ruPlaySound( mAlertSound, 0 );
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
            ItemPlace * dp = mDetonatorPlace[i];

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
            ItemPlace * dp = mDetonatorPlace[i];

            if( dp->IsPickedByPlayer() ) {
                pPlayer->SetActionText( StringBuilder() << GetKeyName( pPlayer->mKeyUse ) << pPlayer->mLocalization.GetString( "putItem") );
            }
        }

        if( ruIsKeyHit( pPlayer->mKeyUse )) {
            for( int i = 0; i < 4; i++ ) {
                ItemPlace * dp = mDetonatorPlace[i];

                if( dp->IsPickedByPlayer() ) {
                    bool placed = dp->PlaceItem( pPlayer->mInventory.GetItemSelectedForUse() );

                    if( placed ) {
                        // 1st: Explosives
                        // 2nd: Detonator
                        // 3rd: Wires
                        // 4th: Ready to explode
                        if( dp->GetPlaceType() == Item::Type::Explosives ) {
                            ruShowNode( mExplosivesModels[i] );
                            dp->SetPlaceType( Item::Type::Detonator );
                        } else if( dp->GetPlaceType() == Item::Type::Detonator ) {
                            ruShowNode( mDetonatorModels[i] );
                            dp->SetPlaceType( Item::Type::Wires );
                        } else if( dp->GetPlaceType() == Item::Type::Wires ) {
                            ruShowNode( mWireModels[i] );
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
        ruSetNodePosition( mDetonatorPlace[i]->mObject, ruVector3( 1000, 1000, 1000 ));
        ruHideNode( mWireModels[i] );
        ruHideNode( mExplosivesModels[i] );
        ruHideNode( mDetonatorModels[i] );
        ruHideNode( mExplosivesDummy[i] );
    }
}

void LevelMine::CreateItems() {
    AddItem( mFuel[0] = new Item( GetUniqueObject( "Fuel1" ), Item::Type::FuelCanister ));
    AddItem( mFuel[1] = new Item( GetUniqueObject( "Fuel2" ), Item::Type::FuelCanister ));
}

void LevelMine::OnDeserialize( TextFileStream & in ) {
    in.ReadBoolean( mDetonatorActivated );
    in.ReadFloat( mBeepSoundTiming );
    mEnemy->Deserialize( in );
}

void LevelMine::OnSerialize( TextFileStream & out ) {
    out.WriteBoolean( mDetonatorActivated );
    out.WriteFloat( mBeepSoundTiming );
    mEnemy->Serialize( out );
}
