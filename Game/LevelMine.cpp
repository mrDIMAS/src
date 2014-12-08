#include "LevelMine.h"
#include "Player.h"
#include "ScreenScreamer.h"
#include "CrawlWay.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine() {
	typeNum = 3;

    LoadLocalization( "mine.loc" );

    scene = ruLoadScene( "data/maps/release/mine/mine.scene");

    player->Place( ruGetNodePosition( ruFindInObjectByName( scene, "PlayerPosition" )));

	ruVector3 placePos = ruGetNodePosition( ruFindInObjectByName( scene, "PlayerPosition" ));
	ruVector3 playerPos = player->GetCurrentPosition();

    player->SetObjective( localization.GetString( "objective1" ));

    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note1" ), localization.GetString( "note1Desc" ), localization.GetString( "note1" ) ) );
    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note2" ), localization.GetString( "note2Desc" ), localization.GetString( "note2" ) ) );
    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note3" ), localization.GetString( "note3Desc" ), localization.GetString( "note3" ) ) );
    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note4" ), localization.GetString( "note4Desc" ), localization.GetString( "note4" ) ) );
    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note5" ), localization.GetString( "note5Desc" ), localization.GetString( "note5" ) ) );
    AddSheet( new Sheet( ruFindInObjectByName( scene, "Note6" ), localization.GetString( "note6Desc" ), localization.GetString( "note6" ) ) );

    screamerZone = ruFindInObjectByName( scene, "Screamer1" );
    screamerZone2 = ruFindInObjectByName( scene, "Screamer2" );
    stoneFallZone = ruFindInObjectByName( scene, "StoneFallZone" );

    newLevelZone = ruFindInObjectByName( scene, "NewLevel" );

    ruSetAudioReverb( 8 );

    AddSound( music = ruLoadMusic( "data/music/chapter2.ogg" ));

    concreteWall = ruFindInObjectByName( scene, "ConcreteWall" );
    deathZone = ruFindInObjectByName( scene, "DeadZone" );
    detonator = ruFindInObjectByName( scene, "Detonator" );

    AddSound( alertSound = ruLoadSound3D( "data/sounds/alert.ogg" ));
    ruAttachSound( alertSound, detonator );

    AddSound( explosionSound = ruLoadSound3D( "data/sounds/blast.ogg" ));
    ruSetSoundReferenceDistance( explosionSound, 10 );

    detonatorActivated = 0;

    explosionFlashAnimator = 0;

    player->SetPlaceDescription( localization.GetString( "placeDesc" ) );

    // Create detonator places
    AddItemPlace( detonatorPlace[0] = new ItemPlace( ruFindInObjectByName( scene, "DetonatorPlace1" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[1] = new ItemPlace( ruFindInObjectByName( scene, "DetonatorPlace2" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[2] = new ItemPlace( ruFindInObjectByName( scene, "DetonatorPlace3" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[3] = new ItemPlace( ruFindInObjectByName( scene, "DetonatorPlace4" ), Item::Explosives ));

    // Create explosives
    AddItem( explosives[ 0 ] = new Item( ruFindInObjectByName( scene, "Explosives1" ), Item::Explosives ));
    AddItem( explosives[ 1 ] = new Item( ruFindInObjectByName( scene, "Explosives2" ), Item::Explosives ));
    AddItem( explosives[ 2 ] = new Item( ruFindInObjectByName( scene, "Explosives3" ), Item::Explosives ));
    AddItem( explosives[ 3 ] = new Item( ruFindInObjectByName( scene, "Explosives4" ), Item::Explosives ));

    // Create detonators
    AddItem( detonators[ 0 ] = new Item( ruFindInObjectByName( scene, "Detonator1" ), Item::Detonator ));
    AddItem( detonators[ 1 ] = new Item( ruFindInObjectByName( scene, "Detonator2" ), Item::Detonator ));
    AddItem( detonators[ 2 ] = new Item( ruFindInObjectByName( scene, "Detonator3" ), Item::Detonator ));
    AddItem( detonators[ 3 ] = new Item( ruFindInObjectByName( scene, "Detonator4" ), Item::Detonator ));

    // Create wires
    AddItem( wires[ 0 ] = new Item( ruFindInObjectByName( scene, "Wire1" ), Item::Wires ));
    AddItem( wires[ 1 ] = new Item( ruFindInObjectByName( scene, "Wire2" ), Item::Wires ));
    AddItem( wires[ 2 ] = new Item( ruFindInObjectByName( scene, "Wire3" ), Item::Wires ));
    AddItem( wires[ 3 ] = new Item( ruFindInObjectByName( scene, "Wire4" ), Item::Wires ));

    wireModels[0] = ruFindInObjectByName( scene, "WireModel1" );
    wireModels[1] = ruFindInObjectByName( scene, "WireModel2" );
    wireModels[2] = ruFindInObjectByName( scene, "WireModel3" );
    wireModels[3] = ruFindInObjectByName( scene, "WireModel4" );

    detonatorModels[0] = ruFindInObjectByName( scene, "DetonatorModel1" );
    detonatorModels[1] = ruFindInObjectByName( scene, "DetonatorModel2" );
    detonatorModels[2] = ruFindInObjectByName( scene, "DetonatorModel3" );
    detonatorModels[3] = ruFindInObjectByName( scene, "DetonatorModel4" );

    explosivesModels[0] = ruFindInObjectByName( scene, "ExplosivesModel1" );
    explosivesModels[1] = ruFindInObjectByName( scene, "ExplosivesModel2" );
    explosivesModels[2] = ruFindInObjectByName( scene, "ExplosivesModel3" );
    explosivesModels[3] = ruFindInObjectByName( scene, "ExplosivesModel4" );

    findItemsZone = ruFindInObjectByName( scene, "FindItemsZone" );

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

    player->SetFootsteps( FootstepsType::Rock );

    AddCrawlWay( new CrawlWay( ruFindInObjectByName( scene, "CrawlBegin" ), ruFindInObjectByName( scene, "CrawlEnd" ), ruFindInObjectByName( scene, "CrawlEnter" ),
                               ruFindInObjectByName( scene, "CrawlBeginLeavePoint"), ruFindInObjectByName( scene, "CrawlEndLeavePoint")));
    AddLadder( new Ladder( ruFindInObjectByName( scene, "LadderBegin" ), ruFindInObjectByName( scene, "LadderEnd" ), ruFindInObjectByName( scene, "LadderEnter" ),
                           ruFindInObjectByName( scene, "LadderBeginLeavePoint"), ruFindInObjectByName( scene, "LadderEndLeavePoint")));
    AddDoor( new Door( ruFindInObjectByName( scene, "Door1" ), 90 ));

    ruPlaySound( music );

    stages[ "EnterRockFallZoneWallExp" ] = false;
    stages[ "EnterScreamerDone" ] = false;
    stages[ "EnterScreamer2Done" ] = false;
    stages[ "ConcreteWallExp" ] = false;
    stages[ "FindObjectObjectiveSet" ] = false;
    stages[ "FoundObjectsForExplosion" ] = false;

	// create paths
	Path path; path.ScanSceneForPath( scene, "Path" );
	Path pathOnUpperLevel; pathOnUpperLevel.ScanSceneForPath( scene, "PathOnUpperLevel" );
	Path pathUpperRight; pathUpperRight.ScanSceneForPath( scene, "PathUpperRight" );
	Path pathUpperLeft; pathUpperLeft.ScanSceneForPath( scene, "PathUpperLeft" );
	Path pathToRoom; pathToRoom.ScanSceneForPath( scene, "PathToRoom" );
	Path pathUpperRightTwo; pathUpperRightTwo.ScanSceneForPath( scene, "PathUpperRightTwo" );

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
	ruSetNodePosition( enemy->body, ruGetNodePosition( ruFindInObjectByName( scene, "EnemyPosition" )));

	ruSetCameraSkybox( player->camera->cameraNode, 0 );
}

LevelMine::~LevelMine() {
	delete enemy;
// FreeSoundSource( music );
}

void LevelMine::Show() {
    Level::Show();

    ruPlaySound( music );
}

void LevelMine::Hide() {
    Level::Hide();

    ruPauseSound( music );
}

void LevelMine::DoScenario() {
    if( Level::curLevelID != LevelName::L2Mine ) {
        return;
    }

	enemy->Think();

	ruSetAmbientColor( ruVector3( 0.08, 0.08, 0.08 ));

	PlayAmbientSounds();

    if( !stages[ "EnterRockFallZoneWallExp" ] ) {
        if( player->IsInsideZone( stoneFallZone )) {
            ruUnfreeze( ruFindByName( "StoneFall" ) );

            stages[ "EnterRockFallZoneWallExp" ] = true;
        }
    }

    if( !stages[ "EnterScreamerDone" ] ) {
        if( player->IsInsideZone( screamerZone )) {
            screamer->DoPeriods( 1 );

            stages[ "EnterScreamerDone" ] = true;
        }
    }

    if( !stages[ "EnterScreamer2Done" ] ) {
        if( player->IsInsideZone( screamerZone2 )) {
            screamer->DoPeriods( 2 );

            stages[ "EnterScreamer2Done" ] = true;
        }
    }

    if( !stages[ "FindObjectObjectiveSet" ] ) {
        if( !stages[ "FoundObjectsForExplosion" ] ) {
            if( player->IsInsideZone( findItemsZone )) {
                player->SetObjective( localization.GetString( "objective2" ) );

                stages[ "FindObjectObjectiveSet" ] = true;
            }
        } else {
            stages[ "FindObjectObjectiveSet" ] = true;
        }
    }

    if( explosionFlashAnimator ) {
        explosionFlashAnimator->Update();
    }

    if( !stages[ "ConcreteWallExp" ] ) {
        static int loops = 60;

        if( player->nearestPicked == detonator ) {
            ruDrawGUIText( localization.GetString( "detonator" ), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );

            if( ruIsKeyHit( player->keyUse ) && readyExplosivesCount >= 4 && !detonatorActivated ) {
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

                ruVector3 vec = ( ruGetNodePosition( concreteWall ) - player->GetCurrentPosition() ).Normalize() * 20;

                ruSetNodePosition( ruFindByName("Rock1"), ruGetNodePosition( ruFindByName("Rock1Pos")));
                ruSetNodePosition( ruFindByName("Rock2"), ruGetNodePosition( ruFindByName("Rock2Pos")));
                ruSetNodePosition( ruFindByName("Rock3"), ruGetNodePosition( ruFindByName("Rock3Pos")));

                ruNodeHandle flash = ruCreateLight();
                ruAttachNode( flash, ruFindByName( "ExplosionFlash" ));
                ruSetLightColor( flash, ruVector3( 255 / 255.0f, 200 / 255.0f, 160 / 255.0f ));
                explosionFlashAnimator = new LightAnimator( flash, 0.25, 30, 1.1 );
                explosionFlashAnimator->lat = LightAnimator::LAT_OFF;

                if( player->IsInsideZone( deathZone )) {
                    player->Damage( 1000 );
                    player->Move( vec, 1 );
                }
            }

            if( ruGetElapsedTimeInSeconds( beepSoundTimer ) > beepSoundTiming ) { // every 1 sec
                beepSoundTiming -= 0.05f;

                ruRestartTimer( beepSoundTimer );

                ruPlaySound( alertSound, 0 );
            }
        }
    }

    if( player->IsInsideZone( newLevelZone )) {
        Level::Change( LevelName::L3ResearchFacility );

        return;
    }

    UpdateExplodeSequence();
}

void LevelMine::UpdateExplodeSequence() {
    if( readyExplosivesCount < 4 ) {
        readyExplosivesCount = 0;

        for( int i = 0; i < 4; i++ ) {
            ItemPlace * dp = detonatorPlace[i];

            if( dp->GetPlaceType() == -1 ) {
                readyExplosivesCount++;
            }

            if( readyExplosivesCount >= 4 ) {
                player->SetObjective( localization.GetString( "objective4" ) );
            }
        }
    }

    static int totalNeededObjects = 0;

    if( totalNeededObjects < 12 ) {
        totalNeededObjects = 0;
        for( auto item : player->inventory.items ) {
            if( item->type == Item::Wires || item->type == Item::Explosives || item->type == Item::Detonator ) {
                totalNeededObjects++;
            }

            if( totalNeededObjects >= 12 ) {
                stages[ "FindObjectObjectiveSet" ] = true;

                player->SetObjective( localization.GetString( "objective3" ) );
            }
        }
    }

    if( player->inventory.forUse ) {
        for( int i = 0; i < 4; i++ ) {
            ItemPlace * dp = detonatorPlace[i];

            if( dp->IsPickedByPlayer() ) {
                ruDrawGUIText( Format( player->localization.GetString( "putItem"), GetKeyName( player->keyUse )).c_str(), ruGetResolutionWidth() / 2 - 256, ruGetResolutionHeight() - 200, 512, 128, gui->font, ruVector3( 255, 0, 0 ), 1 );
            }
        }

        if( ruIsKeyHit( player->keyUse )) {
            for( int i = 0; i < 4; i++ ) {
                ItemPlace * dp = detonatorPlace[i];

                if( dp->IsPickedByPlayer() ) {
                    bool placed = dp->PlaceItem( player->inventory.forUse );

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

void LevelMine::CleanUpExplodeArea() {
    for( int i = 0; i < 4; i++ ) {
        ruSetNodePosition( detonatorPlace[i]->object, ruVector3( 1000, 1000, 1000 ));
        ruHideNode( wireModels[i] );
        ruHideNode( explosivesModels[i] );
        ruHideNode( detonatorModels[i] );

        ruHideNode( ruFindInObjectByName( scene, "ExplosivesModel5") );
        ruHideNode( ruFindInObjectByName( scene, "ExplosivesModel6") );
        ruHideNode( ruFindInObjectByName( scene, "ExplosivesModel7") );
        ruHideNode( ruFindInObjectByName( scene, "ExplosivesModel8") );
    }
}

void LevelMine::CreateItems() {
    AddItem( fuel[0] = new Item( ruFindInObjectByName( scene, "Fuel1" ), Item::FuelCanister ));
    AddItem( fuel[1] = new Item( ruFindInObjectByName( scene, "Fuel2" ), Item::FuelCanister ));
}

void LevelMine::OnDeserialize( TextFileStream & in ) {
    in.ReadBoolean( detonatorActivated );
    in.ReadFloat( beepSoundTiming );
	enemy->Deserialize( in );
}

void LevelMine::OnSerialize( TextFileStream & out ) {
    out.WriteBoolean( detonatorActivated );
    out.WriteFloat( beepSoundTiming );
	enemy->Serialize( out );
}
