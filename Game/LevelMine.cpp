#include "LevelMine.h"
#include "Player.h"
#include "ScreenScreamer.h"
#include "CrawlWay.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine()
{
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

    screamerZone = GetUniqueObject( "Screamer1" );
    screamerZone2 = GetUniqueObject( "Screamer2" );
    stoneFallZone = GetUniqueObject( "StoneFallZone" );

    newLevelZone = GetUniqueObject( "NewLevel" );

    ruSetAudioReverb( 8 );

    AddSound( music = ruLoadMusic( "data/music/chapter2.ogg" ));

    concreteWall = GetUniqueObject( "ConcreteWall" );
    deathZone = GetUniqueObject( "DeadZone" );
    detonator = GetUniqueObject( "Detonator" );

    AddSound( alertSound = ruLoadSound3D( "data/sounds/alert.ogg" ));
    ruAttachSound( alertSound, detonator );

    AddSound( explosionSound = ruLoadSound3D( "data/sounds/blast.ogg" ));
    ruSetSoundReferenceDistance( explosionSound, 10 );

    detonatorActivated = 0;

    explosionFlashAnimator = 0;

    pPlayer->SetPlaceDescription( mLocalization.GetString( "placeDesc" ) );

    // Create detonator places
    AddItemPlace( detonatorPlace[0] = new ItemPlace( GetUniqueObject( "DetonatorPlace1" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[1] = new ItemPlace( GetUniqueObject( "DetonatorPlace2" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[2] = new ItemPlace( GetUniqueObject( "DetonatorPlace3" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[3] = new ItemPlace( GetUniqueObject( "DetonatorPlace4" ), Item::Explosives ));

    // Create explosives
    AddItem( explosives[ 0 ] = new Item( GetUniqueObject( "Explosives1" ), Item::Explosives ));
    AddItem( explosives[ 1 ] = new Item( GetUniqueObject( "Explosives2" ), Item::Explosives ));
    AddItem( explosives[ 2 ] = new Item( GetUniqueObject( "Explosives3" ), Item::Explosives ));
    AddItem( explosives[ 3 ] = new Item( GetUniqueObject( "Explosives4" ), Item::Explosives ));

    // Create detonators
    AddItem( detonators[ 0 ] = new Item( GetUniqueObject( "Detonator1" ), Item::Detonator ));
    AddItem( detonators[ 1 ] = new Item( GetUniqueObject( "Detonator2" ), Item::Detonator ));
    AddItem( detonators[ 2 ] = new Item( GetUniqueObject( "Detonator3" ), Item::Detonator ));
    AddItem( detonators[ 3 ] = new Item( GetUniqueObject( "Detonator4" ), Item::Detonator ));

    // Create wires
    AddItem( wires[ 0 ] = new Item( GetUniqueObject( "Wire1" ), Item::Wires ));
    AddItem( wires[ 1 ] = new Item( GetUniqueObject( "Wire2" ), Item::Wires ));
    AddItem( wires[ 2 ] = new Item( GetUniqueObject( "Wire3" ), Item::Wires ));
    AddItem( wires[ 3 ] = new Item( GetUniqueObject( "Wire4" ), Item::Wires ));

    wireModels[0] = GetUniqueObject( "WireModel1" );
    wireModels[1] = GetUniqueObject( "WireModel2" );
    wireModels[2] = GetUniqueObject( "WireModel3" );
    wireModels[3] = GetUniqueObject( "WireModel4" );

    detonatorModels[0] = GetUniqueObject( "DetonatorModel1" );
    detonatorModels[1] = GetUniqueObject( "DetonatorModel2" );
    detonatorModels[2] = GetUniqueObject( "DetonatorModel3" );
    detonatorModels[3] = GetUniqueObject( "DetonatorModel4" );

    explosivesModels[0] = GetUniqueObject( "ExplosivesModel1" );
    explosivesModels[1] = GetUniqueObject( "ExplosivesModel2" );
    explosivesModels[2] = GetUniqueObject( "ExplosivesModel3" );
    explosivesModels[3] = GetUniqueObject( "ExplosivesModel4" );

    findItemsZone = GetUniqueObject( "FindItemsZone" );

    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine1.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine2.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine3.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine4.ogg" ));
    AddAmbientSound( ruLoadSound3D( "data/sounds/ambient/mine/ambientmine5.ogg" ));

    explosionTimer = ruCreateTimer();

    beepSoundTimer = ruCreateTimer();
    beepSoundTiming = 1.0f;

    CreateItems();

    readyExplosivesCount = 0;

    pPlayer->SetFootsteps( FootstepsType::Rock );

    AddCrawlWay( new CrawlWay( GetUniqueObject( "CrawlBegin" ), GetUniqueObject( "CrawlEnd" ), GetUniqueObject( "CrawlEnter" ),
                               GetUniqueObject( "CrawlBeginLeavePoint"), GetUniqueObject( "CrawlEndLeavePoint")));
    AddLadder( new Ladder( GetUniqueObject( "LadderBegin" ), GetUniqueObject( "LadderEnd" ), GetUniqueObject( "LadderEnter" ),
                           GetUniqueObject( "LadderBeginLeavePoint"), GetUniqueObject( "LadderEndLeavePoint")));
    AddDoor( new Door( GetUniqueObject( "Door1" ), 90 ));

    ruPlaySound( music );

    stages[ "EnterRockFallZoneWallExp" ] = false;
    stages[ "EnterScreamerDone" ] = false;
    stages[ "EnterScreamer2Done" ] = false;
    stages[ "ConcreteWallExp" ] = false;
    stages[ "FindObjectObjectiveSet" ] = false;
    stages[ "FoundObjectsForExplosion" ] = false;

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
    path.vertices[17]->AddEdge( pathOnUpperLevel.vertices[0] );
    pathOnUpperLevel.vertices[6]->AddEdge( pathUpperRight.vertices[0] );
    pathOnUpperLevel.vertices[8]->AddEdge( pathUpperLeft.vertices[0] );
    path.vertices[18]->AddEdge( pathToRoom.vertices[0] );
    pathOnUpperLevel.vertices[12]->AddEdge( pathUpperRightTwo.vertices[0] );

    // concatenate all paths
    vector<GraphVertex*> allPaths;
    allPaths.insert( allPaths.end(), path.vertices.begin(), path.vertices.end() );
    allPaths.insert( allPaths.end(), pathOnUpperLevel.vertices.begin(), pathOnUpperLevel.vertices.end() );
    allPaths.insert( allPaths.end(), pathUpperRight.vertices.begin(), pathUpperRight.vertices.end() );
    allPaths.insert( allPaths.end(), pathUpperLeft.vertices.begin(), pathUpperLeft.vertices.end() );
    allPaths.insert( allPaths.end(), pathToRoom.vertices.begin(), pathToRoom.vertices.end() );
    allPaths.insert( allPaths.end(), pathUpperRightTwo.vertices.begin(), pathUpperRightTwo.vertices.end() );

    vector< GraphVertex* > patrolPoints;
    patrolPoints.push_back( path.vertices[0] );
    patrolPoints.push_back( path.vertices[ path.vertices.size() - 1 ] );
    patrolPoints.push_back( pathOnUpperLevel.vertices[0] );
    patrolPoints.push_back( pathOnUpperLevel.vertices[ pathOnUpperLevel.vertices.size() - 1 ] );
    patrolPoints.push_back( pathUpperRight.vertices[0] );
    patrolPoints.push_back( pathUpperRight.vertices[ pathUpperRight.vertices.size() - 1 ] );
    patrolPoints.push_back( pathUpperLeft.vertices[0] );
    patrolPoints.push_back( pathUpperLeft.vertices[ pathUpperLeft.vertices.size() - 1 ] );
    patrolPoints.push_back( pathUpperRightTwo.vertices[0] );
    patrolPoints.push_back( pathUpperRightTwo.vertices[ pathUpperRightTwo.vertices.size() - 1 ] );
    patrolPoints.push_back( pathToRoom.vertices[0] );
    patrolPoints.push_back( pathToRoom.vertices[ pathToRoom.vertices.size() - 1 ] );

    enemy = new Enemy( "data/models/ripper/ripper.scene", allPaths, patrolPoints );
    ruSetNodePosition( enemy->body, ruGetNodePosition( GetUniqueObject( "EnemyPosition" )));

    ruSetCameraSkybox( pPlayer->mpCamera->mNode, 0 );

	DoneInitialization();
}

LevelMine::~LevelMine()
{
    delete enemy;
// FreeSoundSource( music );
}

void LevelMine::Show()
{
    Level::Show();

    ruPlaySound( music );
}

void LevelMine::Hide()
{
    Level::Hide();

    ruPauseSound( music );
}

void LevelMine::DoScenario()
{
    if( Level::curLevelID != LevelName::L2Mine )
        return;

    enemy->Think();

    ruSetAmbientColor( ruVector3( 0.08, 0.08, 0.08 ));

    PlayAmbientSounds();

    if( !stages[ "EnterRockFallZoneWallExp" ] ) {
        if( pPlayer->IsInsideZone( stoneFallZone )) {
            ruUnfreeze( ruFindByName( "StoneFall" ) );

            stages[ "EnterRockFallZoneWallExp" ] = true;
        }
    }

    if( !stages[ "EnterScreamerDone" ] ) {
        if( pPlayer->IsInsideZone( screamerZone )) {
            screamer->DoPeriods( 1 );

            stages[ "EnterScreamerDone" ] = true;
        }
    }

    if( !stages[ "EnterScreamer2Done" ] ) {
        if( pPlayer->IsInsideZone( screamerZone2 )) {
            screamer->DoPeriods( 2 );

            stages[ "EnterScreamer2Done" ] = true;
        }
    }

    if( !stages[ "FindObjectObjectiveSet" ] ) {
        if( !stages[ "FoundObjectsForExplosion" ] ) {
            if( pPlayer->IsInsideZone( findItemsZone )) {
                pPlayer->SetObjective( mLocalization.GetString( "objective2" ) );

                stages[ "FindObjectObjectiveSet" ] = true;
            }
        } else
            stages[ "FindObjectObjectiveSet" ] = true;
    }

    if( explosionFlashAnimator )
        explosionFlashAnimator->Update();

    if( !stages[ "ConcreteWallExp" ] ) {
        static int loops = 60;

        if( pPlayer->mNearestPickedNode == detonator ) {
            ruDrawGUIText( mLocalization.GetString( "detonator" ), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );

            if( ruIsKeyHit( pPlayer->mKeyUse ) && readyExplosivesCount >= 4 && !detonatorActivated ) {
                detonatorActivated = 1;

                ruRestartTimer( explosionTimer );
            }
        }

        if( detonatorActivated ) {
            if( ruGetElapsedTimeInSeconds( explosionTimer ) >= 10.0f ) {
                detonatorActivated = 0;

                ruPlaySound( explosionSound );

                ruSetSoundPosition( explosionSound, ruGetNodePosition( concreteWall ) );

                stages[ "ConcreteWallExp" ] = true;

                ruSetNodePosition( concreteWall, ruVector3( 10000, 10000, 10000 ));

                CleanUpExplodeArea();

                ruVector3 vec = ( ruGetNodePosition( concreteWall ) - pPlayer->GetCurrentPosition() ).Normalize() * 20;

                ruSetNodePosition( ruFindByName("Rock1"), ruGetNodePosition( ruFindByName("Rock1Pos")));
                ruSetNodePosition( ruFindByName("Rock2"), ruGetNodePosition( ruFindByName("Rock2Pos")));
                ruSetNodePosition( ruFindByName("Rock3"), ruGetNodePosition( ruFindByName("Rock3Pos")));

                ruNodeHandle flash = ruCreateLight();
                ruAttachNode( flash, ruFindByName( "ExplosionFlash" ));
                ruSetLightColor( flash, ruVector3( 255 / 255.0f, 200 / 255.0f, 160 / 255.0f ));
                explosionFlashAnimator = new LightAnimator( flash, 0.25, 30, 1.1 );
                explosionFlashAnimator->lat = LightAnimator::LAT_OFF;

                if( pPlayer->IsInsideZone( deathZone )) {
                    pPlayer->Damage( 1000 );
                    pPlayer->Move( vec, 1 );
                }
            }

            if( ruGetElapsedTimeInSeconds( beepSoundTimer ) > beepSoundTiming ) { // every 1 sec
                beepSoundTiming -= 0.05f;

                ruRestartTimer( beepSoundTimer );

                ruPlaySound( alertSound, 0 );
            }
        }
    }

    if( pPlayer->IsInsideZone( newLevelZone )) {
        Level::Change( LevelName::L3ResearchFacility );

        return;
    }

    UpdateExplodeSequence();
}

void LevelMine::UpdateExplodeSequence()
{
    if( readyExplosivesCount < 4 ) {
        readyExplosivesCount = 0;

        for( int i = 0; i < 4; i++ ) {
            ItemPlace * dp = detonatorPlace[i];

            if( dp->GetPlaceType() == -1 )
                readyExplosivesCount++;

            if( readyExplosivesCount >= 4 )
                pPlayer->SetObjective( mLocalization.GetString( "objective4" ) );
        }
    }

    static int totalNeededObjects = 0;

    if( totalNeededObjects < 12 ) {
        totalNeededObjects = 0;
        for( auto item : pPlayer->mInventory.mItemList ) {
            if( item->mType == Item::Wires || item->mType == Item::Explosives || item->mType == Item::Detonator )
                totalNeededObjects++;

            if( totalNeededObjects >= 12 ) {
                stages[ "FindObjectObjectiveSet" ] = true;

                pPlayer->SetObjective( mLocalization.GetString( "objective3" ) );
            }
        }
    }

    if( pPlayer->mInventory.mpItemForUse ) {
        for( int i = 0; i < 4; i++ ) {
            ItemPlace * dp = detonatorPlace[i];

            if( dp->IsPickedByPlayer() )
                ruDrawGUIText( Format( pPlayer->mLocalization.GetString( "putItem"), GetKeyName( pPlayer->mKeyUse )).c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, pGUI->mFont, ruVector3( 255, 0, 0 ), 1 );
        }

        if( ruIsKeyHit( pPlayer->mKeyUse )) {
            for( int i = 0; i < 4; i++ ) {
                ItemPlace * dp = detonatorPlace[i];

                if( dp->IsPickedByPlayer() ) {
                    bool placed = dp->PlaceItem( pPlayer->mInventory.mpItemForUse );

                    if( placed ) {
                        // 1st: Explosives
                        // 2nd: Detonator
                        // 3rd: Wires
                        // 4th: Ready to explode
                        if( dp->GetPlaceType() == Item::Explosives ) {
                            ruShowNode( explosivesModels[i] );
                            dp->SetPlaceType( Item::Detonator );
                        } else if( dp->GetPlaceType() == Item::Detonator ) {
                            ruShowNode( detonatorModels[i] );
                            dp->SetPlaceType( Item::Wires );
                        } else if( dp->GetPlaceType() == Item::Wires ) {
                            ruShowNode( wireModels[i] );
                            dp->SetPlaceType( -1 );
                        }
                    }
                }
            }
        }
    }
}

void LevelMine::CleanUpExplodeArea()
{
    for( int i = 0; i < 4; i++ ) {
        ruSetNodePosition( detonatorPlace[i]->mObject, ruVector3( 1000, 1000, 1000 ));
        ruHideNode( wireModels[i] );
        ruHideNode( explosivesModels[i] );
        ruHideNode( detonatorModels[i] );

        ruHideNode( GetUniqueObject( "ExplosivesModel5") );
        ruHideNode( GetUniqueObject( "ExplosivesModel6") );
        ruHideNode( GetUniqueObject( "ExplosivesModel7") );
        ruHideNode( GetUniqueObject( "ExplosivesModel8") );
    }
}

void LevelMine::CreateItems()
{
    AddItem( fuel[0] = new Item( GetUniqueObject( "Fuel1" ), Item::FuelCanister ));
    AddItem( fuel[1] = new Item( GetUniqueObject( "Fuel2" ), Item::FuelCanister ));
}

void LevelMine::OnDeserialize( TextFileStream & in )
{
    in.ReadBoolean( detonatorActivated );
    in.ReadFloat( beepSoundTiming );
    enemy->Deserialize( in );
}

void LevelMine::OnSerialize( TextFileStream & out )
{
    out.WriteBoolean( detonatorActivated );
    out.WriteFloat( beepSoundTiming );
    enemy->Serialize( out );
}
