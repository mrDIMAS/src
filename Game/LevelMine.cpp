#include "LevelMine.h"
#include "Player.h"
#include "ScreenScreamer.h"
#include "CrawlWay.h"
#include "Pathfinder.h"
#include "Utils.h"

LevelMine::LevelMine() {
	typeNum = 3;

    LoadLocalization( "mine.loc" );

    scene = LoadScene( "data/maps/release/mine/mine.scene");

    SetPosition( player->body, GetPosition( FindInObjectByName( scene, "PlayerPosition" )));

	Vector3 placePos = GetPosition( FindInObjectByName( scene, "PlayerPosition" ));
	Vector3 playerPos = GetPosition( player->body );

    player->SetObjective( localization.GetString( "objective1" ));

    AddSheet( new Sheet( FindInObjectByName( scene, "Note1" ), localization.GetString( "note1Desc" ), localization.GetString( "note1" ) ) );
    AddSheet( new Sheet( FindInObjectByName( scene, "Note2" ), localization.GetString( "note2Desc" ), localization.GetString( "note2" ) ) );
    AddSheet( new Sheet( FindInObjectByName( scene, "Note3" ), localization.GetString( "note3Desc" ), localization.GetString( "note3" ) ) );
    AddSheet( new Sheet( FindInObjectByName( scene, "Note4" ), localization.GetString( "note4Desc" ), localization.GetString( "note4" ) ) );
    AddSheet( new Sheet( FindInObjectByName( scene, "Note5" ), localization.GetString( "note5Desc" ), localization.GetString( "note5" ) ) );
    AddSheet( new Sheet( FindInObjectByName( scene, "Note6" ), localization.GetString( "note6Desc" ), localization.GetString( "note6" ) ) );

    screamerZone = FindInObjectByName( scene, "Screamer1" );
    screamerZone2 = FindInObjectByName( scene, "Screamer2" );
    stoneFallZone = FindInObjectByName( scene, "StoneFallZone" );

    newLevelZone = FindInObjectByName( scene, "NewLevel" );

    SetReverb( 8 );

    AddSound( music = CreateMusic( "data/music/chapter2.ogg" ));

    concreteWall = FindInObjectByName( scene, "ConcreteWall" );
    deathZone = FindInObjectByName( scene, "DeadZone" );
    detonator = FindInObjectByName( scene, "Detonator" );

    AddSound( alertSound = CreateSound3D( "data/sounds/alert.ogg" ));
    AttachSound( alertSound, detonator );

    AddSound( explosionSound = CreateSound3D( "data/sounds/blast.ogg" ));
    SetSoundReferenceDistance( explosionSound, 10 );

    detonatorActivated = 0;

    explosionFlashAnimator = 0;

    player->SetPlaceDescription( localization.GetString( "placeDesc" ) );

    // Create detonator places
    AddItemPlace( detonatorPlace[0] = new ItemPlace( FindInObjectByName( scene, "DetonatorPlace1" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[1] = new ItemPlace( FindInObjectByName( scene, "DetonatorPlace2" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[2] = new ItemPlace( FindInObjectByName( scene, "DetonatorPlace3" ), Item::Explosives ));
    AddItemPlace( detonatorPlace[3] = new ItemPlace( FindInObjectByName( scene, "DetonatorPlace4" ), Item::Explosives ));

    // Create explosives
    AddItem( explosives[ 0 ] = new Item( FindInObjectByName( scene, "Explosives1" ), Item::Explosives ));
    AddItem( explosives[ 1 ] = new Item( FindInObjectByName( scene, "Explosives2" ), Item::Explosives ));
    AddItem( explosives[ 2 ] = new Item( FindInObjectByName( scene, "Explosives3" ), Item::Explosives ));
    AddItem( explosives[ 3 ] = new Item( FindInObjectByName( scene, "Explosives4" ), Item::Explosives ));

    // Create detonators
    AddItem( detonators[ 0 ] = new Item( FindInObjectByName( scene, "Detonator1" ), Item::Detonator ));
    AddItem( detonators[ 1 ] = new Item( FindInObjectByName( scene, "Detonator2" ), Item::Detonator ));
    AddItem( detonators[ 2 ] = new Item( FindInObjectByName( scene, "Detonator3" ), Item::Detonator ));
    AddItem( detonators[ 3 ] = new Item( FindInObjectByName( scene, "Detonator4" ), Item::Detonator ));

    // Create wires
    AddItem( wires[ 0 ] = new Item( FindInObjectByName( scene, "Wire1" ), Item::Wires ));
    AddItem( wires[ 1 ] = new Item( FindInObjectByName( scene, "Wire2" ), Item::Wires ));
    AddItem( wires[ 2 ] = new Item( FindInObjectByName( scene, "Wire3" ), Item::Wires ));
    AddItem( wires[ 3 ] = new Item( FindInObjectByName( scene, "Wire4" ), Item::Wires ));

    wireModels[0] = FindInObjectByName( scene, "WireModel1" );
    wireModels[1] = FindInObjectByName( scene, "WireModel2" );
    wireModels[2] = FindInObjectByName( scene, "WireModel3" );
    wireModels[3] = FindInObjectByName( scene, "WireModel4" );

    detonatorModels[0] = FindInObjectByName( scene, "DetonatorModel1" );
    detonatorModels[1] = FindInObjectByName( scene, "DetonatorModel2" );
    detonatorModels[2] = FindInObjectByName( scene, "DetonatorModel3" );
    detonatorModels[3] = FindInObjectByName( scene, "DetonatorModel4" );

    explosivesModels[0] = FindInObjectByName( scene, "ExplosivesModel1" );
    explosivesModels[1] = FindInObjectByName( scene, "ExplosivesModel2" );
    explosivesModels[2] = FindInObjectByName( scene, "ExplosivesModel3" );
    explosivesModels[3] = FindInObjectByName( scene, "ExplosivesModel4" );

    findItemsZone = FindInObjectByName( scene, "FindItemsZone" );

    AddAmbientSound( CreateSound3D( "data/sounds/ambient/mine/ambientmine1.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/mine/ambientmine2.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/mine/ambientmine3.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/mine/ambientmine4.ogg" ));
    AddAmbientSound( CreateSound3D( "data/sounds/ambient/mine/ambientmine5.ogg" ));

    explosionTimer = CreateTimer();

    beepSoundTimer = CreateTimer();
    beepSoundTiming = 1.0f;

    CreateItems();

    readyExplosivesCount = 0;

    player->SetFootsteps( FootstepsType::Rock );

    AddCrawlWay( new CrawlWay( FindInObjectByName( scene, "CrawlBegin" ), FindInObjectByName( scene, "CrawlEnd" ), FindInObjectByName( scene, "CrawlEnter" ),
                               FindInObjectByName( scene, "CrawlBeginLeavePoint"), FindInObjectByName( scene, "CrawlEndLeavePoint")));
    AddLadder( new Ladder( FindInObjectByName( scene, "LadderBegin" ), FindInObjectByName( scene, "LadderEnd" ), FindInObjectByName( scene, "LadderEnter" ),
                           FindInObjectByName( scene, "LadderBeginLeavePoint"), FindInObjectByName( scene, "LadderEndLeavePoint")));
    AddDoor( new Door( FindInObjectByName( scene, "Door1" ), 90 ));

    PlaySoundSource( music );

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
	SetPosition( enemy->body, GetPosition( FindInObjectByName( scene, "EnemyPosition" )));

	SetSkybox( player->camera->cameraNode, 0 );
}

LevelMine::~LevelMine() {
	delete enemy;
// FreeSoundSource( music );
}

void LevelMine::Show() {
    Level::Show();

    PlaySoundSource( music );
}

void LevelMine::Hide() {
    Level::Hide();

    PauseSoundSource( music );
}

void LevelMine::DoScenario() {
    if( Level::curLevelID != LevelName::L2Mine ) {
        return;
    }

	enemy->Think();

	SetAmbientColor( Vector3( 0.08, 0.08, 0.08 ));

	PlayAmbientSounds();

    if( !stages[ "EnterRockFallZoneWallExp" ] ) {
        if( IsNodeInside( player->body, stoneFallZone )) {
            Unfreeze( FindByName( "StoneFall" ) );

            stages[ "EnterRockFallZoneWallExp" ] = true;
        }
    }

    if( !stages[ "EnterScreamerDone" ] ) {
        if( IsNodeInside( player->body, screamerZone )) {
            screamer->DoPeriods( 1 );

            stages[ "EnterScreamerDone" ] = true;
        }
    }

    if( !stages[ "EnterScreamer2Done" ] ) {
        if( IsNodeInside( player->body, screamerZone2 )) {
            screamer->DoPeriods( 2 );

            stages[ "EnterScreamer2Done" ] = true;
        }
    }

    if( !stages[ "FindObjectObjectiveSet" ] ) {
        if( !stages[ "FoundObjectsForExplosion" ] ) {
            if( IsNodeInside( player->body, findItemsZone )) {
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
            DrawGUIText( localization.GetString( "detonator" ), GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );

            if( IsKeyHit( player->keyUse ) && readyExplosivesCount >= 4 && !detonatorActivated ) {
                detonatorActivated = 1;

                RestartTimer( explosionTimer );
            }
        }

        if( detonatorActivated ) {
            if( GetElapsedTimeInSeconds( explosionTimer ) >= 10.0f ) {
                detonatorActivated = 0;

                PlaySoundSource( explosionSound );

                SetSoundPosition( explosionSound, GetPosition( concreteWall ) );

                stages[ "ConcreteWallExp" ] = true;

                SetPosition( concreteWall, Vector3( 10000, 10000, 10000 ));

                CleanUpExplodeArea();

                Vector3 vec = ( GetPosition( concreteWall ) - GetPosition( player->body )).Normalize() * 20;

                SetPosition( FindByName("Rock1"), GetPosition( FindByName("Rock1Pos")));
                SetPosition( FindByName("Rock2"), GetPosition( FindByName("Rock2Pos")));
                SetPosition( FindByName("Rock3"), GetPosition( FindByName("Rock3Pos")));

                NodeHandle flash = CreateLight();
                Attach( flash, FindByName( "ExplosionFlash" ));
                SetLightColor( flash, Vector3( 255 / 255.0f, 200 / 255.0f, 160 / 255.0f ));
                explosionFlashAnimator = new LightAnimator( flash, 0.25, 30, 1.1 );
                explosionFlashAnimator->lat = LightAnimator::LAT_OFF;

                if( IsNodeInside( player->body, deathZone )) {
                    player->Damage( 1000 );
                    Move( player->body, vec );
                }
            }

            if( GetElapsedTimeInSeconds( beepSoundTimer ) > beepSoundTiming ) { // every 1 sec
                beepSoundTiming -= 0.05f;

                RestartTimer( beepSoundTimer );

                PlaySoundSource( alertSound, 0 );
            }
        }
    }

    if( IsNodeInside( player->body, newLevelZone )) {
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
                DrawGUIText( Format( player->localization.GetString( "putItem"), GetKeyName( player->keyUse )).c_str(), GetResolutionWidth() / 2 - 256, GetResolutionHeight() - 200, 512, 128, gui->font, Vector3( 255, 0, 0 ), 1 );
            }
        }

        if( IsKeyHit( player->keyUse )) {
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
                            ShowNode( explosivesModels[i] );
                            dp->SetPlaceType( Item::Detonator );
                        } else if( dp->GetPlaceType() == Item::Detonator ) {
                            ShowNode( detonatorModels[i] );
                            dp->SetPlaceType( Item::Wires );
                        } else if( dp->GetPlaceType() == Item::Wires ) {
                            ShowNode( wireModels[i] );
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
        SetPosition( detonatorPlace[i]->object, Vector3( 1000, 1000, 1000 ));
        HideNode( wireModels[i] );
        HideNode( explosivesModels[i] );
        HideNode( detonatorModels[i] );

        HideNode( FindInObjectByName( scene, "ExplosivesModel5") );
        HideNode( FindInObjectByName( scene, "ExplosivesModel6") );
        HideNode( FindInObjectByName( scene, "ExplosivesModel7") );
        HideNode( FindInObjectByName( scene, "ExplosivesModel8") );
    }
}

void LevelMine::CreateItems() {
    AddItem( fuel[0] = new Item( FindInObjectByName( scene, "Fuel1" ), Item::FuelCanister ));
    AddItem( fuel[1] = new Item( FindInObjectByName( scene, "Fuel2" ), Item::FuelCanister ));
}

void LevelMine::OnDeserialize( TextFileStream & in )
{
    in.ReadBoolean( detonatorActivated );
    in.ReadFloat( beepSoundTiming );
}

void LevelMine::OnSerialize( TextFileStream & out )
{
    out.WriteBoolean( detonatorActivated );
    out.WriteFloat( beepSoundTiming );
}
